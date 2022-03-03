#pragma once

#include "iplugin.hpp"


class AddPhoto : public IPlugIn {
public:

  AddPhoto(
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


