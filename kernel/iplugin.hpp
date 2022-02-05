#pragma once

#include <rapidjson/document.h>
#include <vector>
#include <string>
#include <memory>

class IPlugIn {
public:
  std::vector<uint8_t> m_SideData;

  IPlugIn(const rapidjson::Document& config) {
    m_Config.CopyFrom(config, m_Config.GetAllocator());
  }

  virtual void GetName(std::string& pluginName)       const = 0;
  virtual void GetStatus(std::string& statusMessage)  const = 0;
  virtual bool Start(const std::string& uuid) = 0;

  virtual bool Valid() { return true;  }
  virtual bool Stop()  { return false; }
  virtual bool Pause() { return false; }
  virtual bool Kill()  { return false; }

  virtual void AddSideData(const uint8_t * data, size_t size) { m_SideData.assign(data, data+size); }

private:
  rapidjson::Document m_Config;
};

using IPlugInPtr = std::shared_ptr<IPlugIn>;


