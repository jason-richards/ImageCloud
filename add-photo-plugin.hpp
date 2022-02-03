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


  virtual bool Start() { sleep(10); return true; }
  virtual bool Stop()  { return false; }
  virtual bool Pause() { return false; }
  virtual bool Kill()  { return false; }


  void AddSideData(
    const uint8_t * data,
    size_t size
  ) override {
    IPlugIn::AddSideData(data, size);
    Artifacts::ProbeExif EP = Artifacts::ProbeExif(data, size);

    std::string format;

    if (EP.GetCompression(format)) {
      std::cout << "Compression is: \'" << format << "\'\n";
    }

    //Artifacts::ProbeMedia MP = Artifacts::ProbeMedia(data, size);

    //std::string format;

    //if (MP.GetFormat(format)) {
    //  std::cout << "Format is: " << format << std::endl;
    //}
  }
};


