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
  Initialize(
    std::string&
  ) override;


  bool
  Start() override;
};

