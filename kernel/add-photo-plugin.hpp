#pragma once

#include "iplugin.hpp"


class AddPhoto : public IPlugIn {
public:

  AddPhoto(
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


