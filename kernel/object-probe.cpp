#include "object-probe.hpp"

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
namespace Objects {

class Probe {
public:
  std::vector<cv::Rect> m_Objects;


  Probe(
    const std::string& cascadePath,
    const std::vector<uint8_t>& photo
  ) {
    cv::CascadeClassifier face_cascade(cascadePath);

    cv::Mat decodedImage = cv::imdecode(photo, CV_LOAD_IMAGE_GRAYSCALE);
    if (decodedImage.data == nullptr) {
      throw std::runtime_error("Error decoding photo.");
    }

    cv::equalizeHist(decodedImage, decodedImage);
    face_cascade.detectMultiScale(decodedImage, m_Objects, 1.3, 3, cv::CASCADE_SCALE_IMAGE);
    decodedImage.release();
  }


  void 
  GetObjectRectangles(
    std::vector<Miso::ObjectRectangleT>& faces
  ) {
    std::for_each(m_Objects.begin(), m_Objects.end(),
      [&faces](cv::Rect face) {
        faces.push_back(Miso::ObjectRectangleT{.x=face.x, .y=face.y, .width=face.width, .height=face.height});
      }
    );
  }

};


/* Create a new Object probe.
 *
 * @param std::string - Object cascade path.
 * @param uint8_t * - Pointer to photo data.
 * @param size_t - Image size.
 * @result ProbePtr - Object probe pointer.
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
 * @param ProbePtr - Object probe pointer.
 * @param std::vector<ObjectRectangleT> - Vector of face rectangles
 * @result bool - True if faces were found.
 */
void
GetObjectRectangles(
  ProbePtr FP,
  std::vector<Miso::ObjectRectangleT>& faces
) {
  FP->GetObjectRectangles(faces);
}

} // namespace Objects
} // namespace Artifacts

