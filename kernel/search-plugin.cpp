#include <filesystem>
#include <algorithm>
#include <iomanip>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "search-plugin.hpp"
#include "file-io.hpp"

#define DEFAULT_SEARCH_RADIUS_MILES 10

Search::Search(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request), m_RadiusMiles(DEFAULT_SEARCH_RADIUS_MILES) {
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

  if (request.HasMember("radius") && request["radius"].IsString()) {
    m_RadiusMiles = std::stof(request["radius"].GetString());
  }
}


float
GetDistanceKM(
  float latitude[2],
  float longitude[2]
) {
  constexpr float deglen = 110.25;
  float x = latitude[0] - latitude[1];
  float y = (longitude[0] - longitude[1]) * cos(latitude[1]);
  return deglen * std::sqrt(x*x + y*y);
}


float
GetDistanceMiles(
  float latitude[2],
  float longitude[2]
) {
  constexpr float kmToM = 0.62137;
  return kmToM * GetDistanceKM(latitude, longitude);
}


bool
Search::Initialize(
  Responder responder
) {
  auto path = m_Config["storage"].as<std::string>() + "/Photos";

  /*
   *  Step 1: Filter source directories by Date, Year, Month, and/or Day.
   */
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


  /*
   *  Step 2: Filter images by Name, and/or Location
   */
  std::list<std::string> files;
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
        auto ifs = InputFile::Create(file.path());
        rapidjson::IStreamWrapper isw(ifs->Get());
        rapidjson::Document img_json;
        rapidjson::ParseResult ok = img_json.ParseStream(isw);
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
          if (!img_json.HasMember("location") || !img_json["location"].IsString()) {
            continue;
          }

          const char * location = img_json["location"].GetString();

          float latitude[2] = {0, 0}, longitude[2] = {0, 0};
          if (std::sscanf(location, "%f, %f", &latitude[0], &longitude[0]) != 2) {
            continue;
          }

          if (std::sscanf(m_Location->c_str(), "%f, %f", &latitude[1], &longitude[1]) != 2) {
            continue;
          }

          float calculatedDistance = GetDistanceMiles(latitude, longitude);

          if (calculatedDistance > m_RadiusMiles) {
            continue;
          }
        }
      }

      files.push_back(std::string(path.stem()));
    }
  }


  /*
   *  Step 3: Generate JSON response string.
   */
  auto jsonResponse = std::string("{\"status\" : \"OK\", \"results\" : [");

  for (const auto& file : files) {
    if (&file != &files.front()) {
      jsonResponse += ", ";
    }

    jsonResponse += "\"";
    jsonResponse += file;
    jsonResponse += "\"";
  }

  jsonResponse += "]}";

  return responder(jsonResponse.data(), jsonResponse.size(), false);
}


