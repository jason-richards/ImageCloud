#pragma once

#include "iplugin.hpp"


class AddPhoto : public IPlugIn {
public:

  AddPhoto(
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


