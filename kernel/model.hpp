#pragma once

#include <memory>


class ModelData;
using ModelDataPtr = std::unique_ptr<ModelData>;


class Model {
  ModelDataPtr m_ModelData;

public:
  using ModelPtr  = std::shared_ptr<Model>;
  using ImageData = std::vector<uint8_t>;

  Model();


  static ModelPtr &
  GetInstance();


  void
  SaveState(
    std::string& jsonState
  );


  bool
  RestoreState(
    const std::string& jsonState
  );

  void
  LoadModel(
    const std::string& where
  );


  void
  PersistModel(
    const std::string& where
  );


  bool
  Identify(
    const std::string& label,
    const std::vector<uint8_t>& image
  );
};


