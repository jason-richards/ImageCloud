#pragma once

#include <yaml-cpp/yaml.h>
#include <rapidjson/document.h>
#include <vector>
#include <string>
#include <memory>

class IPlugIn {
public:
  std::vector<uint8_t> m_SideData;

  IPlugIn(const YAML::Node& config, const rapidjson::Document& request) : m_Config(config) {
    m_Request.CopyFrom(request, m_Request.GetAllocator());
  }

  virtual void GetName(std::string& pluginName)       const = 0;
  virtual void GetStatus(std::string& statusMessage)  const = 0;
  virtual bool Start(const std::string& uuid) = 0;

  virtual bool Valid() { return true;  }
  virtual bool Stop()  { return false; }
  virtual bool Pause() { return false; }
  virtual bool Kill()  { return false; }

  virtual void AddSideData(const uint8_t * data, size_t size) { m_SideData.assign(data, data+size); }

protected:
  const YAML::Node& m_Config;
  rapidjson::Document m_Request;
};

using IPlugInPtr = std::shared_ptr<IPlugIn>;


