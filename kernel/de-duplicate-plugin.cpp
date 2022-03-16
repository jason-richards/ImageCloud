#include <filesystem>
#include <iostream>

#include "de-duplicate-plugin.hpp"
#include "miso.hpp"


DeDuplicate::DeDuplicate(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {};


bool
DeDuplicate::Initialize(
  Responder responder
) {
  auto r = std::string("{\"status\" : \"OK\"}");
  return responder(r.data(), r.size(), false);
}


bool
DeDuplicate::Start() {
  auto path = m_Config["storage"].as<std::string>() + "/Photos";

  /*
   */

  std::map<std::string, std::list<std::string> > duplicates;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
        if (!file.is_regular_file()) {
          continue;
        }

        auto path = std::filesystem::path(file.path());

        if (path.empty()) {
          continue;
        }

        if (!path.has_extension()) {
          continue;
        }

        std::string extension = std::string(path.extension());
        std::transform(extension.begin(), extension.end(), extension.begin(),
          [](unsigned char c){ return std::tolower(c); });

        if (extension.compare(".json")) {
          continue;
        }

        auto meta_path = std::filesystem::absolute(path);

        std::string signature;
        Miso::MisoPtr MP = Miso::CreateContext(meta_path);
        Miso::GetSignature(MP, signature);

        if (duplicates.count(signature) > 0) {
          std::string photo_path = path.replace_extension("");
          if (std::filesystem::exists(photo_path)) {
            MP.reset();
            std::filesystem::remove_all(photo_path);
            std::filesystem::remove_all(meta_path);
          }
        }

        duplicates[signature].push_back(meta_path);
      }
    }
  }

  return true;
}

