#pragma once

#include <memory>
#include <string>
#include <vector>
#include "miso.hpp"

namespace Artifacts {
namespace Dlib {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;

/* Create a new Dlib probe.
 *
 * @param std::string& - Image filename.
 * @result ProbePtr - Object probe pointer.
 */
ProbePtr
CreateProbe(
  const std::string&
);


/* Obtain a vector of all the objects found in the image.
 *
 * @param ProbePtr - Object probe pointer
 * @param std::vector<ObjectRectangleT> - Vector of object rectangles
 */
void
GetObjectRectangles(
  ProbePtr,
  std::vector<Miso::ObjectRectangleT>&
);


} // namespace Dlib
} // namespace Artifacts

