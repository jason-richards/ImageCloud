#include <filesystem>
#include <ctime>
#include <iomanip>

#include "add-photo-plugin.hpp"

#include "media-probe.hpp"
#include "exif-probe.hpp"
#include "sha256-probe.hpp"
#include "object-probe.hpp"
#include "miso.hpp"
#include "file-io.hpp"


AddPhoto::AddPhoto(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {};


bool
AddPhoto::Initialize(
  Responder responder
) {
  auto r = std::string("{\"status\" : \"OK\"}");
  return responder(r.data(), r.size(), false);
}


std::string
PrepDirectory(
  const std::string& base_path,
  const std::string& timestamp
) {
  struct tm timeinfo;                                                                                                   
  std::stringstream ss;                                                                                                 
                                                                                                                        
  if (!strptime(timestamp.c_str(), "%Y:%m:%d %H:%M:%S", &timeinfo) &&
      !strptime(timestamp.c_str(), "%Y/%m/%d %H:%M:%S", &timeinfo))
  {
    std::time_t t = std::time(0);                                                                                       
    ss << base_path << "/Photos/" << std::put_time(std::localtime(&t), "%Y-%m-%d");                                     
  } else {                                                                                                              
    ss << base_path << "/Photos/" << std::put_time(&timeinfo, "%Y-%m-%d");                                              
  }                                                                                                                     
                                                                                                                        
  std::string new_path = ss.str();
  if (!std::filesystem::exists(new_path)) {
    if (!std::filesystem::create_directory(new_path)) {
      throw std::runtime_error("Unable to create Photo storage: " + new_path);
    }
  }

  return new_path;
}


void
ObjectSearch(
  const YAML::Node& objects,
  const auto& process_objects
) {
  if (!objects) {
    return;
  }

  if (objects.IsSequence()) {
    for (YAML::const_iterator it = objects.begin(); it != objects.end(); ++it) {
      const YAML::Node& b = *it;
      YAML::Node::const_iterator iter;
      YAML::Node node;
      for(auto iter = b.begin(); iter != b.end(); iter++ ) {
        node[iter->first.as<std::string>()] = iter->second.as<std::string>();
      }
      process_objects(node);
    }
  }
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

  Miso::MisoPtr MP = Miso::CreateContext();
  Miso::SetUUID(MP, uuid);
  Miso::SetSignature(MP, signature);
  Miso::SetTimeStamp(MP, timestamp);
  Miso::SetCompression(MP, compression);
  Miso::SetLocation(MP, location);
  Miso::SetWidth(MP, width);
  Miso::SetHeight(MP, height);

  auto ProcessObjects =
    [&](const YAML::Node& object) {
      std::vector<Miso::ObjectRectangleT> rects;
      Artifacts::Objects::ProbePtr FP = Artifacts::Objects::CreateProbe(object["cascade"].as<std::string>(), m_SideData);
      GetObjectRectangles(FP, rects);
      Miso::SetObjectRectangles(MP, object["name"].as<std::string>(), rects);
    };

  ObjectSearch(m_Config["objects"], ProcessObjects);

  auto path = PrepDirectory(m_Config["storage"].as<std::string>(), timestamp);

  auto manifest = OutputFile::Create(std::string(path + "/" + uuid + ".json"));
  Miso::Write(MP, manifest->Get());

  auto photo = OutputFile::Create(std::string(path + "/" + uuid));
  photo->write(reinterpret_cast<const char *>(m_SideData.data()), m_SideData.size());

  return true;
}

