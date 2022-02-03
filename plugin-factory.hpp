#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <functional>
#include <memory>

#include "add-photo-plugin.hpp"

class PlugInFactory {
public:

  using PlugInBuilder = std::function<IPlugInPtr(const rapidjson::Document&)>;


  PlugInFactory() {
    RegisterBuilder(std::string("AddPhoto"), 
      [](const rapidjson::Document& config)->IPlugInPtr {
        return std::make_shared<AddPhoto>(config);
      }
    );
  }


  IPlugInPtr
  GetPlugIn(
    const std::vector<uint8_t>& request,
    std::string& errors
  ) {
    rapidjson::Document config;

    rapidjson::ParseResult ok = config.Parse(reinterpret_cast<const char*>(request.data()));
    if (!ok) {
      errors = std::string("ERROR: ");
      errors += GetParseError_En(ok.Code());
      return nullptr;
    }

    if (!config.HasMember("plugin") || !config["plugin"].IsString()) {
      errors = std::string("ERROR: Missing \"plugin\" directive in request JSON");
      return nullptr;
    }
 
    std::string name = config["plugin"].GetString();

    PlugInBuilder builder;
    if (!FindBuilder(name, builder)) {
      errors = std::string("ERROR: Plugin not found: ");
      errors += name;
      return nullptr;
    }

    IPlugInPtr plugin = builder(config);

    if (!plugin) {
      errors = std::string("ERROR: Plugin not found: ");
      errors += name;
      return nullptr;
    }

    if (config.HasMember("bytes")) {
      if (config["bytes"].IsUint64()) {
        int64_t size = config["bytes"].GetUint64();
        size_t offset = request.size() - size;
        plugin->AddSideData(request.data() + offset, size);
      }
    }

    return plugin;
  }


  void
  RegisterBuilder(
    const std::string& name,
    PlugInBuilder builder
  ) {
    m_BuilderMap[name] = builder;
  }


  bool
  FindBuilder(
    const std::string& name,
    PlugInBuilder& builder
  ) {
    if (m_BuilderMap.find(name) == m_BuilderMap.end()) {
      return false;
    }

    builder = m_BuilderMap[name];
    return true;
  }

  std::map<std::string,PlugInBuilder > m_BuilderMap;
};

