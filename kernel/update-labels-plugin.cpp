#include <filesystem>

#include "update-labels-plugin.hpp"
#include "model.hpp"
#include "miso.hpp"
#include "file-io.hpp"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#pragma GCC diagnostic pop


UpdateLabels::UpdateLabels(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {};


bool
UpdateLabels::Initialize(
  Responder responder
) {
  auto r = std::string("{\"status\" : \"OK\"}");
  return responder(r.data(), r.size(), false);
}


bool
UpdateLabels::Start() {
  auto path = m_Config["storage"].as<std::string>() + "/Models";

  /*
   * Step 1:  Load all face identity models.
   */
  bool loaded = false;
  cv::Ptr<cv::face::FaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
  for (const auto& file : std::filesystem::directory_iterator(path)) {
    if (!file.is_regular_file()) {
      continue;
    }

    auto path = std::filesystem::path(file.path());

    if (path.empty()) {
      continue;
    }

    if (!path.has_extension()) {
      continue;
    }

    std::string extension = std::string(path.extension());
    std::transform(extension.begin(), extension.end(), extension.begin(),
      [](unsigned char c){ return std::tolower(c); });

    if (extension.compare(".yaml")) {
      continue;
    }

    model->read(path);

    loaded = true;

    break;
  }

  if (!loaded) {
    return false;
  }


  /*
   * Step 2:  Find Some Peeps!
   */
  path = m_Config["storage"].as<std::string>() + "/Photos";
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
        if (!file.is_regular_file()) {
          continue;
        }

        auto path = std::filesystem::path(file.path());

        if (path.empty()) {
          continue;
        }

        if (!path.has_extension()) {
          continue;
        }

        std::string extension = std::string(path.extension());
        std::transform(extension.begin(), extension.end(), extension.begin(),
          [](unsigned char c){ return std::tolower(c); });

        if (extension.compare(".json")) {
          continue;
        }

        auto meta_path = std::filesystem::absolute(path);

        std::string photo_path = path.replace_extension("");
        if (!std::filesystem::exists(photo_path)) {
          continue;
        }

        cv::Mat decodedImage = cv::imread(photo_path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        if (decodedImage.data == nullptr) {
          continue;
        }

        Miso::MisoPtr MP = Miso::CreateContext(meta_path);
        std::vector<Miso::FaceRectangleT> faces;
        Miso::GetFaceRectangles(MP, faces);

        std::vector<std::string> labels;
        for (auto& face : faces) {
          int label = -1;
          double confidence = 0;
          cv::Mat croppedRef(decodedImage, cv::Rect(face.x, face.y, face.width, face.height));
          model->predict(croppedRef, label, confidence);
          auto filename = std::to_string(std::rand()) + ".jpg";
          cv::imwrite(filename, croppedRef);
          labels.push_back(model->getLabelInfo(label).c_str());
        }

        if (labels.size()) {
          Miso::SetFaceLabels(MP, labels);
          auto manifest = OutputFile::Create(meta_path);
          Miso::Write(MP, manifest->Get());
        }
      }
    }
  }

  return true;
}

