#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <functional>
#include <memory>

#include "add-photo-plugin.hpp"
#include "get-photo-plugin.hpp"
#include "identify-plugin.hpp"
#include "search-plugin.hpp"

class PlugInFactory {
public:

  using PlugInBuilder = std::function<IPlugInPtr(const YAML::Node&, const rapidjson::Document&)>;


  PlugInFactory() {
    RegisterBuilder(std::string("AddPhoto"), 
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<AddPhoto>(config, request);
      }
    );

    RegisterBuilder(std::string("GetPhoto"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<GetPhoto>(config, request);
      }
    );

    RegisterBuilder(std::string("Identify"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<Identify>(config, request);
      }
    );

    RegisterBuilder(std::string("Search"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<Search>(config, request);
      }
    );
  }

  void
  SetError(
    const std::string& message,
    std::string& jsonResponse
  ) {
    jsonResponse = std::string("{\"status\" : \"ERROR\", \"message\" : \"");
    jsonResponse += message;
    jsonResponse += "\"}";
  }


  IPlugInPtr
  GetPlugIn(
    const YAML::Node& config,
    const std::vector<uint8_t>& request,
    Responder responder
  ) {
    auto jsonResponse = std::string();

    rapidjson::Document doc;

    rapidjson::ParseResult ok = doc.Parse(reinterpret_cast<const char*>(request.data()));
    if (!ok) {
      SetError(GetParseError_En(ok.Code()), jsonResponse);
      return nullptr;
    }

    if (!doc.HasMember("plugin") || !doc["plugin"].IsString()) {
      SetError(std::string("Missing \"plugin\" directive in request JSON"), jsonResponse);
      return nullptr;
    }
 
    std::string name = doc["plugin"].GetString();

    PlugInBuilder builder;
    if (!FindBuilder(name, builder)) {
      SetError(std::string("Plugin not found."), jsonResponse);
      return nullptr;
    }

    IPlugInPtr plugin = builder(config, doc);

    if (!plugin) {
      SetError(std::string("Plugin not found."), jsonResponse);
      return nullptr;
    }

    if (doc.HasMember("bytes")) {
      if (doc["bytes"].IsUint64()) {
        int64_t size = doc["bytes"].GetUint64();
        size_t offset = request.size() - size;
        plugin->AddSideData(request.data() + offset, size);
      }
    }

    plugin->Initialize(responder);

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

