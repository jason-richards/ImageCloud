#include "face-probe.hpp"

#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#include <iostream>

namespace Artifacts {


class FaceProbe {
public:
  static std::unique_ptr<FaceProbe> m_Instance;

  cv::FileStorage m_FS;
  std::vector<cv::Rect> m_Faces;

  static FaceProbe&
  getInstance(
    const std::string& cascadePath
  ) {
    if (m_Instance == nullptr) {
      m_Instance.reset(new FaceProbe(cascadePath));
    }

    return *m_Instance;
  }
  

  FaceProbe(
    const std::string& cascadePath
  ) {
    m_FS = cv::FileStorage(cascadePath, cv::FileStorage::READ);
  }


  void
  Decode(
    const std::vector<uint8_t>& image
  ) {
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.read(m_FS.getFirstTopLevelNode())) {
      throw std::runtime_error("Error loading cascade.");
    }

    cv::Mat decodedImage = cv::imdecode(image, CV_LOAD_IMAGE_GRAYSCALE);
    if (decodedImage.data == nullptr) {
      throw std::runtime_error("Error decoding image.");
    }

    cv::equalizeHist(decodedImage, decodedImage);
    face_cascade.detectMultiScale(decodedImage, m_Faces);
    decodedImage.release();
  }


  void 
  GetFaceRectangles(
    std::vector<FaceRectangleT>& faces
  ) {
    std::for_each(m_Faces.begin(), m_Faces.end(),
      [&faces](cv::Rect face) {
        faces.push_back(FaceRectangleT{.x=face.x, .y=face.y, .width=face.width, .height=face.height});
      }
    );
  }

};

std::unique_ptr<FaceProbe> FaceProbe::m_Instance = nullptr;


/* Create a new Face probe.
 *
 * @param std::string - Face cascade path.
 * @param uint8_t * - Pointer to image data.
 * @param size_t - Image size.
 * @result FaceProbePtr - Face probe pointer.
 */
FaceProbe&
GetFaceProbe(
  const std::string& cascadePath,
  const std::vector<uint8_t>& image
) {
  FaceProbe& FP = FaceProbe::getInstance(cascadePath);
  FP.Decode(image);
  return FP;
}


/* Obtain a vector of all the faces found in the image.
 *
 * @param FaceProbePtr - Face probe pointer.
 * @param std::vector<FaceRectangleT> - Vector of face rectangles
 * @result bool - True if faces were found.
 */
void
GetFaceRectangles(
  FaceProbe& FP,
  std::vector<FaceRectangleT>& faces
) {
  FP.GetFaceRectangles(faces);
}


} // namespace Artifacts

