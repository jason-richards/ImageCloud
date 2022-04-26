#include <filesystem>
#include <functional>
#include <string>
#include <mutex>

#include "model.hpp"
#include "file-io.hpp"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#pragma GCC diagnostic pop


class ModelData {
public:
  int m_LabelCounter;
  std::mutex m_Mutex;

  cv::Ptr<cv::face::FaceRecognizer> m_Model = cv::face::LBPHFaceRecognizer::create();

  ModelData(
    int startLabel = 0
  ) : m_LabelCounter(startLabel), m_Model(cv::face::LBPHFaceRecognizer::create()) {}

  int
  GetNextLabel() {
    return m_LabelCounter++;
  }
};


static Model::ModelPtr g_InstancePtr = std::make_shared<Model>();


Model::Model() {
  m_ModelData.reset(new ModelData());
}


Model::ModelPtr &
Model::GetInstance() {
  return g_InstancePtr;
}


void
Model::LoadModel(
  const std::string& where
) {
  std::unique_lock<std::mutex> lock(m_ModelData->m_Mutex);
  try {
    m_ModelData->m_Model->read(where + "/model.yaml");
    m_ModelData->m_LabelCounter = m_ModelData->m_Model->getLabelsByString("").size();
  } catch (...) {

  };
}


void
Model::PersistModel(
  const std::string& where
) {
  std::unique_lock<std::mutex> lock(m_ModelData->m_Mutex);
  m_ModelData->m_Model->write(where + "/model.yaml");
}


bool
Model::Identify(
  const std::string& name,
  const std::vector<uint8_t>& image
) {
  std::unique_lock<std::mutex> lock(m_ModelData->m_Mutex);

  std::vector<int> ids = m_ModelData->m_Model->getLabelsByString(name);

  int id = ids.size() == 0 ? m_ModelData->GetNextLabel() : ids[0];
  std::vector<int> labels(1, id);

  cv::Mat decodedImage = cv::imdecode(image, CV_LOAD_IMAGE_GRAYSCALE);
  if (decodedImage.data == nullptr) {
    throw std::runtime_error("Error decoding photo.");
  }

  std::vector<cv::Mat> photos;
  photos.push_back(decodedImage);

  if (m_ModelData->m_Model->empty()) {
    m_ModelData->m_Model->train(photos, labels);
  } else {
    m_ModelData->m_Model->update(photos, labels);
  }

  m_ModelData->m_Model->setLabelInfo(id, name);

  decodedImage.release();

  return true;
}


