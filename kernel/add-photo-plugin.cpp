#include <iostream>

#include "add-photo-plugin.hpp"

#include "media-probe.hpp"
#include "exif-probe.hpp"
#include "sha256-probe.hpp"


AddPhoto::AddPhoto(const rapidjson::Document& config) : IPlugIn(config) {}


void
AddPhoto::GetName(
  std::string& pluginName
) const {
  pluginName = std::string("AddPhoto");
}


void
AddPhoto::GetStatus(
  std::string& statusMessage
) const {
  statusMessage = std::string("OK");
}

/*
  {
    "UUID": "1A80E667-8554-11EC-B4CF-D93A08136CE5",
    "signature": "0BE4524E3CE0BDCC7B9C72F9C510CE08",
    "timestamp": "2022:01:08 13:51:53.12",
    "compression" : "JPEG Compression", 
    "width": 3264,
    "height": 1836,
    "latitude": "47 deg 52' 0.25\"N ",
    "longitude": "122 deg 12' 59.37\"W ",
    "faces": [{
      "x": 649,
      "y": 215,
      "width": 20,
      "height": 20
    }, {
      "x": 249,
      "y": 115,
      "width": 30,
      "height": 30
    }]
  }

 */
     
bool
AddPhoto::Start(
  const std::string& uuid
) {
  std::string compression;
  Artifacts::ExifProbePtr EP = Artifacts::CreateExifProbe(m_SideData.data(), m_SideData.size());
  if (Artifacts::GetCompression(EP, compression)) {
    std::cout << "Compression: " << compression << "\n";
  }

  int width = 0, height = 0;
  GetWidth(EP, width);
  GetHeight(EP, height);

  Artifacts::SHA256ProbePtr SP = Artifacts::CreateSHA256Probe(m_SideData.data(), m_SideData.size());
  std::string signature;
  if (Artifacts::GetSignature128(SP, signature)) {
    std::cout << "Signature: " << signature << "\n";
  }


  return true;
}

