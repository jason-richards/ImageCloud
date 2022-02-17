#include <filesystem>
#include <functional>
#include <fstream>
#include <string>

#include "identify-plugin.hpp"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#pragma GCC diagnostic pop


Identify::Identify(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {
  if (request.HasMember("name") && request["name"].IsString()) {
    m_Name = request["name"].GetString();
  } else {
    m_Name = std::string("N/A");
  }
}


void
Identify::GetName(
  std::string& pluginName
) const {
  pluginName = std::string("Identify");
}


void
Identify::GetStatus(
  std::string& statusMessage
) const {
  statusMessage = std::string("OK");
}


std::string
PrepDirectory(
  const std::string& base_path
) {
  std::string new_path = base_path + "/Models";
  if (!std::filesystem::exists(new_path)) {
    if (!std::filesystem::create_directory(new_path)) {
      throw std::runtime_error("Unable to create Model storage: " + new_path);
    }
  }

  return new_path;
}


bool
Identify::Start(
  const std::string& uuid
) {
  auto path = PrepDirectory(m_Config["storage"].as<std::string>());

  cv::Mat decodedImage = cv::imdecode(m_SideData, CV_LOAD_IMAGE_GRAYSCALE);
  if (decodedImage.data == nullptr) {
    throw std::runtime_error("Error decoding image.");
  }

  std::vector<int> labels;
  std::vector<cv::Mat> images;
  int label = std::hash<std::string>{}(m_Name);
  std::string labelString = std::to_string(label);

  labels.push_back(label);
  images.push_back(decodedImage);

  cv::Ptr<cv::face::FaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();

  model->train(images, labels);
  model->setLabelInfo(label, m_Name);
  model->write(path + "/" + labelString + ".yaml");

  std::ofstream image(std::string(path + "/" + labelString + ".jpg"), std::ios::binary);
  image.write(reinterpret_cast<const char *>(m_SideData.data()), m_SideData.size());
  image.close();

  decodedImage.release();

  return true;
}


