#pragma once

#include "iplugin.hpp"

#define WRITE_BLOCKSIZE 131072

class GetThumbnail : public IPlugIn {
public:
  std::string m_UUID;


  GetThumbnail(
    const YAML::Node& config,
    const rapidjson::Document& request
  );


  bool
  Initialize(
    Responder
  ) override;

};

