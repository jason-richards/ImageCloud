#include <cstdio>
#include <filesystem>

#include "dlib-probe.hpp"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <dlib/pixel.h>

namespace Artifacts {
namespace Dlib {

class Probe {
public:
  std::vector<dlib::rectangle> m_Objects;


  Probe(
    const std::string& filename
  ) {
    dlib::array2d<unsigned char> img;
    dlib::load_image(img, filename);
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    m_Objects = detector(img);
  }


  void 
  GetObjectRectangles(
    std::vector<Miso::ObjectRectangleT>& faces
  ) {
    std::for_each(m_Objects.begin(), m_Objects.end(),
      [&faces](dlib::rectangle face) {
        auto point = face.tl_corner();
        faces.push_back(
          Miso::ObjectRectangleT{
            .x=static_cast<int>(point.x()),
            .y=static_cast<int>(point.y()),
            .width=static_cast<int>(face.width()),
            .height=static_cast<int>(face.height())
          }
        );
      }
    );
  }

};


/* Create a new Object probe.
 *
 * @param const std::string& - Photo
 * @result ProbePtr - Object probe pointer.
 */
ProbePtr
CreateProbe(
  const std::string& photo
) {
  return std::make_shared<Probe>(photo);
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

} // namespace Dlib
} // namespace Artifacts

