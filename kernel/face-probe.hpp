#pragma once

#include <memory>
#include <string>
#include <vector>
#include "miso.hpp"

namespace Artifacts {
namespace Face {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;

/* Create a new Face probe.
 *
 * @param std::string - Face cascade path.
 * @param std::vector<uint8_t> - Image.
 * @result ProbePtr - Face probe pointer.
 */
ProbePtr
CreateProbe(
  const std::string&,
  const std::vector<uint8_t>&
);


/* Obtain a vector of all the faces found in the image.
 *
 * @param ProbePtr - Face probe pointer
 * @param std::vector<FaceRectangleT> - Vector of face rectangles
 */
void
GetFaceRectangles(
  ProbePtr,
  std::vector<Miso::FaceRectangleT>&
);


} // namespace Face
} // namespace Artifacts

