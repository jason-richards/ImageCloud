#pragma once

#define _UNICODE

#include <libexif/exif-data.h>
#include <iostream>

#include "iplugin.hpp"
#include "media-probe.hpp"
#include "exif-probe.hpp"


class AddPhoto : public IPlugIn {
public:

  AddPhoto(const rapidjson::Document& config) : IPlugIn(config) {}


  void
  GetName(
    std::string& pluginName
  ) const override {
    pluginName = std::string("AddPhoto");
  }


  void
  GetStatus(
    std::string& statusMessage
  ) const override {
    statusMessage = std::string("OK");
  }


  virtual bool Start() { return true; }
  virtual bool Stop()  { return false; }
  virtual bool Pause() { return false; }
  virtual bool Kill()  { return false; }


  void AddSideData(
    const uint8_t * data,
    size_t size
  ) override {
    IPlugIn::AddSideData(data, size);
    Artifacts::ProbeExif EP = Artifacts::ProbeExif(data, size);
  }
};


