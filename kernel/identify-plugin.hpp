#pragma once
#include "iplugin.hpp"

class Identify : public IPlugIn {
public:
  std::string m_Name;

  Identify(
    const YAML::Node& config,
    const rapidjson::Document& request
  );


  void
  GetName(
    std::string& pluginName
  ) const override;


  void
  GetStatus(
    std::string& statusMessage
  ) const override;


  bool
  Start(
    const std::string& uuid
  ) override;
};

