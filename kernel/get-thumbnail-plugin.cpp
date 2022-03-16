#include <filesystem>

#include "get-thumbnail-plugin.hpp"
#include "exif-probe.hpp"
#include "file-io.hpp"


GetThumbnail::GetThumbnail(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {
  if (request.HasMember("uuid") && request["uuid"].IsString()) {
    m_UUID = request["uuid"].GetString();
    std::transform(m_UUID.begin(), m_UUID.end(), m_UUID.begin(),
      [](unsigned char c){ return std::toupper(c); });
  }
}


bool
GetThumbnail::Initialize(
  Responder responder
) {
  auto pathname = m_Config["storage"].as<std::string>() + "/Photos";

  std::list<std::filesystem::directory_entry> directories;
  for (const auto& entry : std::filesystem::directory_iterator(pathname)) {
    if (entry.is_directory()) {
      std::filesystem::path photo_path {entry.path()/m_UUID};
      if (std::filesystem::exists(photo_path)) {

        /* Step 1:  Read photo file. */
        std::vector<uint8_t> buffer(std::filesystem::file_size(photo_path));
        auto photo = InputFile::Create(photo_path);
        auto& status = photo->read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if (!status) {
          auto r = std::string("{\"status\" : \"ERROR\", \"message\" : \"Photo Not Found.\"}");
          return responder(r.data(), r.size(), false);
        }


        /* Step 2:  Get the EXIF Thumbnail, if possible. */
        std::vector<char> thumbnail;
        Artifacts::Exif::ProbePtr EP = Artifacts::Exif::CreateProbe(buffer.data(), buffer.size());
        if (!Artifacts::Exif::GetThumbnail(EP, thumbnail)) {
          auto r = std::string("{\"status\" : \"ERROR\", \"message\" : \"EXIF Thumbnail Not Found.\"}");
          return responder(r.data(), r.size(), false);
        }


        /* Step 3:  Write JSON response header. */
        auto r = std::string("{\"status\" : \"OK\", \"bytes\" : ");
        r += std::to_string(thumbnail.size());
        r += " }";
        if (!responder(r.data(), r.size(), true)) {
          return false;
        }


        /* Step 4:  Transfer thumbnail. */
        if (!responder(thumbnail.data(), thumbnail.size(), false)) {
          return false;
        }

        return false;
      }
    }
  }

  auto r = std::string("{\"status\" : \"ERROR\", \"message\" : \"File Not Found.\"}");
  return responder(r.data(), r.size(), false);
}


