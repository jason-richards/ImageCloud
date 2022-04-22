#pragma once

#include "iplugin.hpp"


class UpdateLabels : public IPlugIn {
public:

  UpdateLabels(
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


