#pragma once

#include <memory>
#include <string>
#include <vector>
#include "miso.hpp"

namespace Artifacts {
namespace Objects {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;

/* Create a new Object probe.
 *
 * @param std::string - Object cascade path.
 * @param std::vector<uint8_t> - Image.
 * @result ProbePtr - Object probe pointer.
 */
ProbePtr
CreateProbe(
  const std::string&,
  const std::vector<uint8_t>&
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


} // namespace Object
} // namespace Artifacts

