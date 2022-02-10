#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Artifacts {
namespace Face {

class Probe;


/**************
 *  0-------X
 *  |
 *  |  []
 *  Y
 **************/
typedef struct
FaceRectangle {
  int x;
  int y;
  int width;
  int height;
} FaceRectangleT;


/* Create a new Face probe.
 *
 * @param std::string - Face cascade path.
 * @param std::vector<uint8_t> - Image.
 * @result Probe& - Face probe reference.
 */
Probe&
GetProbe(
  const std::string&,
  const std::vector<uint8_t>&
);


/* Obtain a vector of all the faces found in the image.
 *
 * @param const Probe - Face probe reference.
 * @param std::vector<FaceRectangleT> - Vector of face rectangles
 */
void
GetFaceRectangles(
  Probe&,
  std::vector<FaceRectangleT>&
);


} // namespace Face
} // namespace Artifacts

