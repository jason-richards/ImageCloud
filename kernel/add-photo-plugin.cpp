#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iomanip>

#include "add-photo-plugin.hpp"

#include "media-probe.hpp"
#include "exif-probe.hpp"
#include "sha256-probe.hpp"
#include "face-probe.hpp"
#include "miso.hpp"


AddPhoto::AddPhoto(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {};


void
AddPhoto::Initialize(
  std::string& jsonResponse
) {
  jsonResponse = std::string("{\"status\" : \"OK\"}");
}


std::string
PrepDirectory(
  const std::string& base_path,
  const std::string& timestamp
) {
  struct tm timeinfo;
  if (!strptime(timestamp.c_str(), "%Y:%m:%d %H:%M:%S", &timeinfo)) {
    throw std::runtime_error("Invalid Date/Time format.");
  }

  std::stringstream ss;
  ss << base_path << "/Photos/" << std::put_time(&timeinfo, "%Y-%m-%d");

  std::string new_path = ss.str();
  if (!std::filesystem::exists(new_path)) {
    if (!std::filesystem::create_directory(new_path)) {
      throw std::runtime_error("Unable to create Photo storage: " + new_path);
    }
  }

  return new_path;
}


bool
AddPhoto::Start() {
  std::string signature, timestamp, compression, location;
  int width = 0, height = 0;

  std::string uuid = GetID();

  Artifacts::Exif::ProbePtr EP = Artifacts::Exif::CreateProbe(m_SideData.data(), m_SideData.size());
  Artifacts::Exif::GetCompression(EP, compression);
  Artifacts::Exif::GetWidth(EP, width);
  Artifacts::Exif::GetHeight(EP, height);
  Artifacts::Exif::GetLocation(EP, location);
  Artifacts::Exif::GetDateTime(EP, timestamp);
  Artifacts::SHA256::ProbePtr SP = Artifacts::SHA256::CreateProbe(m_SideData.data(), m_SideData.size());
  Artifacts::SHA256::GetSignature128(SP, signature);

  Artifacts::Face::ProbePtr FP = Artifacts::Face::CreateProbe(m_Config["face_cascade"].as<std::string>(), m_SideData);
  std::vector<Miso::FaceRectangleT> faces;
  GetFaceRectangles(FP, faces);

  Miso::MisoPtr MP = Miso::CreateContext();
  Miso::SetUUID(MP, uuid);
  Miso::SetSignature(MP, signature);
  Miso::SetTimeStamp(MP, timestamp);
  Miso::SetCompression(MP, compression);
  Miso::SetLocation(MP, location);
  Miso::SetWidth(MP, width);
  Miso::SetHeight(MP, height);
  Miso::SetFaceRectangles(MP, faces);

  auto path = PrepDirectory(m_Config["storage"].as<std::string>(), timestamp);

  std::ofstream manifest(std::string(path + "/" + uuid + ".json"));
  Miso::Write(MP, manifest);
  manifest.close();

  std::ofstream image(std::string(path + "/" + uuid + ".jpg"));
  image.write(reinterpret_cast<const char *>(m_SideData.data()), m_SideData.size());
  image.close();

  return true;
}

