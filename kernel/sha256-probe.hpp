#pragma once
#include <memory>
#include <string>


namespace Artifacts {
namespace SHA256 {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;


ProbePtr
CreateProbe(
  const uint8_t *,
  size_t
);


bool
GetSignature(
  ProbePtr,
  std::string&
);


bool
GetSignature128(
  ProbePtr,
  std::string&
);


} // namespace SHA256
} // namespace Artifacts

