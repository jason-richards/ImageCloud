#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>

#include "add-photo-plugin.hpp"

#include "media-probe.hpp"
#include "exif-probe.hpp"
#include "sha256-probe.hpp"
#include "face-probe.hpp"
#include "miso.hpp"


AddPhoto::AddPhoto(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {
};


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

     
bool
AddPhoto::Start(
  const std::string& uuid
) {
  std::string signature, timestamp, compression, longitude, latitude; 
  int width = 0, height = 0;

  Artifacts::Exif::ProbePtr EP = Artifacts::Exif::CreateProbe(m_SideData.data(), m_SideData.size());
  Artifacts::Exif::GetCompression(EP, compression);
  Artifacts::Exif::GetWidth(EP, width);
  Artifacts::Exif::GetHeight(EP, height);
  Artifacts::Exif::GetLongitude(EP, longitude);
  Artifacts::Exif::GetLatitude(EP, latitude);
  Artifacts::Exif::GetDateTime(EP, timestamp);

  Artifacts::SHA256::ProbePtr SP = Artifacts::SHA256::CreateProbe(m_SideData.data(), m_SideData.size());
  Artifacts::SHA256::GetSignature128(SP, signature);

  auto& FP = Artifacts::Face::GetProbe(m_Config["face_cascade"].as<std::string>(), m_SideData);
  std::vector<Artifacts::Face::FaceRectangleT> faces;
  GetFaceRectangles(FP, faces);
  for(auto face : faces) {
    std::cout << face.x << ":" << face.y << ", width: " << face.width << ", height: " << face.height << std::endl;
  }

  auto two = Miso::CreateContext();
  Miso::SetUUID(two,   uuid);
  Miso::SetSignature(two, signature);
  Miso::SetTimeStamp(two, timestamp);
  Miso::SetCompression(two,  compression);
  Miso::SetLongitude(two, longitude);
  Miso::SetLatitude(two, latitude);
  Miso::SetWidth(two,  width);
  Miso::SetHeight(two, height);

  std::ofstream out("cow.json");
  Miso::Write(two, out);

  return true;
}

