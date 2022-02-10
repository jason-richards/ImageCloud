#pragma once

#include <memory>

namespace Artifacts {
namespace Media {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;


ProbePtr
CreateProbe(
  const uint8_t * data,
  size_t size
);


bool
GetFormat(
  ProbePtr,
  std::string&
);



} // namespace Media
} // namespace Artifacts

