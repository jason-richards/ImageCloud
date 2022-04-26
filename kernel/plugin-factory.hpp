#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <functional>
#include <memory>

#include "add-photo-plugin.hpp"
#include "get-photo-plugin.hpp"
#include "get-thumbnail-plugin.hpp"
#include "identify-plugin.hpp"
#include "search-plugin.hpp"
#include "de-duplicate-plugin.hpp"
#include "update-labels-plugin.hpp"

class PlugInFactory {
public:

  using PlugInBuilder = std::function<IPlugInPtr(const YAML::Node&, const rapidjson::Document&)>;

  /* The PlugIn Factory.
   */
  PlugInFactory() {
    RegisterBuilder(std::string("AddPhoto"), 
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<AddPhoto>(config, request);
      }
    );

    RegisterBuilder(std::string("DeDuplicate"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<DeDuplicate>(config, request);
      }
    );

    RegisterBuilder(std::string("GetPhoto"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<GetPhoto>(config, request);
      }
    );

    RegisterBuilder(std::string("GetThumb"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<GetThumbnail>(config, request);
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

    RegisterBuilder(std::string("UpdateLabels"),
      [](const YAML::Node& config, const rapidjson::Document& request)->IPlugInPtr {
        return std::make_shared<UpdateLabels>(config, request);
      }
    );
  }


  /* A helper function to build a JSON error response.
   *
   * @param message - An error message to wrap in a JSON response string.
   * @param jsonResponse - A JSON response string.
   */
  inline void
  SetError(
    const std::string& message,
    Responder responder
  ) {
    auto r = std::string();
    r = std::string("{\"status\" : \"ERROR\", \"message\" : \"");
    r += message;
    r += "\"}";
    responder(r.data(), r.size(), false);
  }


  /* Find a plugin builder and execute it.  Write the response in the given responder.
   *
   * @param config - A reference to the global configuration.
   * @param request - A reference to the plugin request.  Usually a JSON string + some binary data.
   * @param responder - The plugin request responder to write response or error responses.
   */
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
      SetError(GetParseError_En(ok.Code()), responder);
      return nullptr;
    }

    if (!doc.HasMember("plugin") || !doc["plugin"].IsString()) {
      SetError(std::string("Missing \"plugin\" directive in request JSON"), responder);
      return nullptr;
    }
 
    std::string name = doc["plugin"].GetString();

    PlugInBuilder builder;
    if (!FindBuilder(name, builder)) {
      SetError(std::string("Plugin not found."), responder);
      return nullptr;
    }

    IPlugInPtr plugin = builder(config, doc);

    if (!plugin) {
      SetError(std::string("Plugin not found."), responder);
      return nullptr;
    }

    if (doc.HasMember("bytes")) {
      if (doc["bytes"].IsUint64()) {
        int64_t size = doc["bytes"].GetUint64();
        size_t offset = request.size() - size;
        plugin->AddSideData(request.data() + offset, size);
      }
    }

    return plugin->Initialize(responder) ? plugin : nullptr;
  }


  /* Register a plugin builder with the builder map by name.
   *
   * @param std::string - The String name of the plugin builder.
   * @param PlugInBuilder - The plugin builder.
   */
  void
  RegisterBuilder(
    const std::string& name,
    PlugInBuilder builder
  ) {
    m_BuilderMap[name] = builder;
  }


  /* Search the builder map by name for a plugin builder.
   *
   * @param std::string - The String name of the plugin builder.
   * @result bool - True if successfully found a builder.
   */
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

