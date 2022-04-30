#pragma once

#include "iplugin.hpp"

class Status : public IPlugIn {
public:
  std::string m_UUID;


  Status(
    const YAML::Node& config,
    const rapidjson::Document& request
  );


  bool
  Initialize(
    Responder
  ) override;

};


