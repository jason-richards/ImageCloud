#pragma once

#include "iplugin.hpp"


class DeDuplicate : public IPlugIn {
public:

  DeDuplicate(
    const YAML::Node& config,
    const rapidjson::Document& request
  );


  bool
  Initialize(
    Responder
  ) override;


  bool
  Start() override;
};


