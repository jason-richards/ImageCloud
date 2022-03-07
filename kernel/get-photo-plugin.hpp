#pragma once

#include "iplugin.hpp"

#define WRITE_BLOCKSIZE 131072

class GetPhoto : public IPlugIn {
public:
  std::string m_UUID;


  GetPhoto(
    const YAML::Node& config,
    const rapidjson::Document& request
  );


  bool
  Initialize(
    Responder
  ) override;

};


