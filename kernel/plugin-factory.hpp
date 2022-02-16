#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <functional>
#include <memory>

#include "add-photo-plugin.hpp"
#include "identify-plugin.hpp"

class PlugInFactory {
public:

  using PlugInBuilder = std::function<IPlugInPtr(const YAML::Node&, const rapidjson::Document&)>;


  PlugInFactory() {
    RegisterBuilder(std::string("AddPhoto"), 
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<AddPhoto>(config, request);
      }
    );

    RegisterBuilder(std::string("Identify"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<Identify>(config, request);
      }
    );
  }


  IPlugInPtr
  GetPlugIn(
    const YAML::Node& config,
    const std::vector<uint8_t>& request,
    std::string& errors
  ) {
    rapidjson::Document doc;

    rapidjson::ParseResult ok = doc.Parse(reinterpret_cast<const char*>(request.data()));
    if (!ok) {
      errors = std::string("ERROR: ");
      errors += GetParseError_En(ok.Code());
      return nullptr;
    }

    if (!doc.HasMember("plugin") || !doc["plugin"].IsString()) {
      errors = std::string("ERROR: Missing \"plugin\" directive in request JSON");
      return nullptr;
    }
 
    std::string name = doc["plugin"].GetString();

    PlugInBuilder builder;
    if (!FindBuilder(name, builder)) {
      errors = std::string("ERROR: Plugin not found: ");
      errors += name;
      return nullptr;
    }

    IPlugInPtr plugin = builder(config, doc);

    if (!plugin) {
      errors = std::string("ERROR: Plugin not found: ");
      errors += name;
      return nullptr;
    }

    if (doc.HasMember("bytes")) {
      if (doc["bytes"].IsUint64()) {
        int64_t size = doc["bytes"].GetUint64();
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

