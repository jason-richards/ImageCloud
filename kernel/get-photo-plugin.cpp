#include <filesystem>

#include "get-photo-plugin.hpp"
#include "file-io.hpp"


GetPhoto::GetPhoto(
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
GetPhoto::Initialize(
  Responder responder
) {
  auto pathname = m_Config["storage"].as<std::string>() + "/Photos";

  std::list<std::filesystem::directory_entry> directories;
  for (const auto& entry : std::filesystem::directory_iterator(pathname)) {
    if (entry.is_directory()) {
      std::filesystem::path photo_path {entry.path()/(m_UUID+".jpg")};
      if (std::filesystem::exists(photo_path)) {

        /* Step 1:  Write JSON response header. */
        auto r = std::string("{\"status\" : \"OK\", \"bytes\" : ");
        r += std::to_string(std::filesystem::file_size(photo_path));
        r += " }";
        if (!responder(r.data(), r.size(), true)) {
          return false;
        }


        /* Step 2:  Transfer photo. */
        std::vector<char> buffer(WRITE_BLOCKSIZE);
        auto photo = InputFile::Create(photo_path);
        while (!photo->eof()) {
          photo->read(buffer.data(), buffer.size());
          size_t bytes = photo->gcount();
          if (!responder(buffer.data(), bytes, photo->eof() ? false : true)) {
            return false;
          }
        }

        return false;
      }
    }
  }

  auto r = std::string("{\"status\" : \"ERROR\", \"message\" : \"File Not Found.\"}");
  return responder(r.data(), r.size(), false);
}


