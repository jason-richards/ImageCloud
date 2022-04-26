#include <filesystem>
#include <functional>
#include <string>

#include "identify-plugin.hpp"
#include "file-io.hpp"
#include "model.hpp"
#include "sha256-probe.hpp"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#pragma GCC diagnostic pop


Identify::Identify(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {
  if (request.HasMember("name") && request["name"].IsString()) {
    m_Name = request["name"].GetString();
  } else {
    m_Name = std::string("N/A");
  }
}


bool
Identify::Initialize(
  Responder responder
) {
  auto r = std::string("{\"status\" : \"OK\"}");
  return responder(r.data(), r.size(), false);
}


std::string
PrepDirectory(
  const std::string& base_path
) {
  std::string new_path = base_path + "/Models";
  if (!std::filesystem::exists(new_path)) {
    if (!std::filesystem::create_directory(new_path)) {
      throw std::runtime_error("Unable to create Model storage: " + new_path);
    }
  }

  return new_path;
}


bool
Identify::Start() {
  Model::ModelPtr& g_ModelInstancePtr = Model::GetInstance();

  std::string path = PrepDirectory(m_Config["storage"].as<std::string>());
  g_ModelInstancePtr->Identify(m_Name, m_SideData);
  g_ModelInstancePtr->PersistModel(m_Config["storage"].as<std::string>() + "/Models");

  std::string facechip_path(path + "/" + m_Name);
  if (!std::filesystem::exists(facechip_path)) {
    if (!std::filesystem::create_directory(facechip_path)) {
      throw std::runtime_error("Unable to create Model facechip path: " + facechip_path);
    }
  }

  std::string facechip_name;
  Artifacts::SHA256::ProbePtr SP = Artifacts::SHA256::CreateProbe(m_SideData.data(), m_SideData.size());
  Artifacts::SHA256::GetSignature128(SP, facechip_name);

  OutputFile::OutputFilePtr photo = OutputFile::Create(std::string(facechip_path + "/" + facechip_name));
  photo->write(reinterpret_cast<const char *>(m_SideData.data()), m_SideData.size());
  return true;
}


