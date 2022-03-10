#include "face-probe.hpp"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#pragma GCC diagnostic pop

namespace Artifacts {
namespace Face {

class Probe {
public:
  static std::unique_ptr<cv::FileStorage> m_FS;
  std::vector<cv::Rect> m_Faces;


  Probe(
    const std::string& cascadePath,
    const std::vector<uint8_t>& photo
  ) {
    if (m_FS == nullptr) {
      m_FS.reset(new cv::FileStorage(cascadePath, cv::FileStorage::READ));
    }

    cv::CascadeClassifier face_cascade;
    if (!face_cascade.read(m_FS->getFirstTopLevelNode())) {
      throw std::runtime_error("Error loading cascade.");
    }

    cv::Mat decodedImage = cv::imdecode(photo, CV_LOAD_IMAGE_GRAYSCALE);
    if (decodedImage.data == nullptr) {
      throw std::runtime_error("Error decoding photo.");
    }

    cv::equalizeHist(decodedImage, decodedImage);
    face_cascade.detectMultiScale(decodedImage, m_Faces, 1.3, 3);
    decodedImage.release();
  }


  void 
  GetFaceRectangles(
    std::vector<Miso::FaceRectangleT>& faces
  ) {
    std::for_each(m_Faces.begin(), m_Faces.end(),
      [&faces](cv::Rect face) {
        faces.push_back(Miso::FaceRectangleT{.x=face.x, .y=face.y, .width=face.width, .height=face.height});
      }
    );
  }

};

std::unique_ptr<cv::FileStorage> Probe::m_FS = nullptr;


/* Create a new Face probe.
 *
 * @param std::string - Face cascade path.
 * @param uint8_t * - Pointer to photo data.
 * @param size_t - Image size.
 * @result ProbePtr - Face probe pointer.
 */
ProbePtr
CreateProbe(
  const std::string& cascadePath,
  const std::vector<uint8_t>& photo
) {
  return std::make_shared<Probe>(cascadePath, photo);
}


/* Obtain a vector of all the faces found in the photo.
 *
 * @param ProbePtr - Face probe pointer.
 * @param std::vector<FaceRectangleT> - Vector of face rectangles
 * @result bool - True if faces were found.
 */
void
GetFaceRectangles(
  ProbePtr FP,
  std::vector<Miso::FaceRectangleT>& faces
) {
  FP->GetFaceRectangles(faces);
}

} // namespace Face
} // namespace Artifacts

