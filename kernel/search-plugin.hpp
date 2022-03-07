#pragma once
#include "iplugin.hpp"

#include <memory>


class Search : public IPlugIn {
public:
  std::unique_ptr<std::string> m_Date;
  std::unique_ptr<std::string> m_Year;
  std::unique_ptr<std::string> m_Month;
  std::unique_ptr<std::string> m_Day;
  std::unique_ptr<std::string> m_Location;
  std::unique_ptr<std::string> m_Name;
  float m_RadiusMiles;


  Search(
    const YAML::Node& config,
    const rapidjson::Document& request
  );


  bool
  Initialize(
    Responder
  ) override;
};

