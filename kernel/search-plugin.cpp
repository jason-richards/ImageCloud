#include "search-plugin.hpp"

#include <filesystem>
#include <algorithm>
#include <iomanip>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>


Search::Search(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {
  if (request.HasMember("date") && request["date"].IsString()) {
    m_Date.reset(new std::string(request["date"].GetString()));
  }

  if (request.HasMember("year")) {
    if (request["year"].IsString()) {
      m_Year.reset(new std::string(request["year"].GetString()));
    } else if (request["year"].IsInt()) {
      m_Year.reset(new std::string(std::to_string(request["year"].GetInt())));
    }
  }

  if (request.HasMember("month")) {
    if (request["month"].IsString()) {
      m_Month.reset(new std::string(request["month"].GetString()));
    } else if (request["month"].IsInt()) {
      m_Month.reset(new std::string(std::to_string(request["month"].GetInt())));
    }
  }

  if (request.HasMember("day")) {
    if (request["day"].IsString()) {
      m_Day.reset(new std::string(request["day"].GetString()));
    } else if (request["day"].IsInt()) {
      m_Day.reset(new std::string(std::to_string(request["day"].GetInt())));
    }
  }

  if (request.HasMember("name") && request["name"].IsString()) {
    m_Name.reset(new std::string(request["name"].GetString()));
  }

  if (request.HasMember("location") && request["location"].IsString()) {
    m_Location.reset(new std::string(request["location"].GetString()));
  }
}


void
Search::GetName(
  std::string& pluginName
) const {
  pluginName = std::string("Search");
}


void
Search::GetStatus(
  std::string& statusMessage
) const {
  statusMessage = std::string("OK");
}


bool
Search::Start(
  const std::string& uuid
) {
  auto path = m_Config["storage"].as<std::string>() + "/Photos";

  std::list<std::filesystem::directory_entry> directories;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      std::tm dir_time = {};
      std::istringstream ss(std::filesystem::path(entry.path()).filename());
      ss >> std::get_time(&dir_time, "%Y-%m-%d");

      std::tm req_time;
      if (m_Date && strptime(m_Date->c_str(), m_Date->c_str(), &req_time)) {
        if (req_time.tm_year != dir_time.tm_year ||
            req_time.tm_mon  != dir_time.tm_mon  ||
            req_time.tm_mday != dir_time.tm_mday)
        {
          continue;
        }
      }

      if (m_Year && strptime(m_Year->c_str(), "%Y", &req_time)) {
        if (req_time.tm_year != dir_time.tm_year) {
          continue;
        }
      }

      if (m_Month && (strptime(m_Month->c_str(), "%m", &req_time) || strptime(m_Month->c_str(), "%B", &req_time))) {
        if (req_time.tm_mon != dir_time.tm_mon) {
          continue;
        }
      }

      if (m_Day && strptime(m_Day->c_str(), "%d", &req_time)) {
        if (req_time.tm_mday != dir_time.tm_mday) {
          continue;
        }
      }

      directories.push_back(entry);
    }
  }

  std::list<std::filesystem::directory_entry> files;
  for (const auto& entry : directories) {
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

      auto filename = path.filename();
      if (m_Name || m_Location) {
        std::ifstream ifs(file.path());
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document img_json;
        rapidjson::ParseResult ok = img_json.ParseStream(isw);
        ifs.close();
        if (!ok) {
          continue;
        }

        if (m_Name) {
          if (!img_json.HasMember("names")) {
            continue;
          }

          const rapidjson::Value& names = img_json["names"];
          if (names.IsArray()) {
            bool found = false;
            for (const auto& n : names.GetArray()) {
              std::string nStr = n.GetString();
              if (nStr.find(m_Name->c_str()) != std::string::npos) {
                found = true;
                break;
              }
            }

            if (!found) {
              continue;
            }
          } else if (names.IsString()) {
            std::string nStr = names.GetString();
            if (nStr.find(m_Name->c_str()) == std::string::npos) {
              continue;
            }
          } else {
            continue;
          }
        }

        if (m_Location) {
          // TODO
        }
      }

      files.push_back(file);
    }

    std::list<std::string> uuids;
    for (const auto& file : files) {
      std::filesystem::path path(file.path());
      uuids.push_back(path.stem());
    }

  }

  // 3. Location Search, if applicable

  return false;
}


