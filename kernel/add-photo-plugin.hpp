#pragma once

#include "iplugin.hpp"


class AddPhoto : public IPlugIn {
public:

  AddPhoto(const rapidjson::Document& config);


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


