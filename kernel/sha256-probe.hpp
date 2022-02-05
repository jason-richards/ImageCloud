#pragma once
#include <memory>
#include <string>


namespace Artifacts {


class SHA256Probe;
using SHA256ProbePtr = std::shared_ptr<SHA256Probe>;


SHA256ProbePtr
CreateSHA256Probe(
  const uint8_t *,
  size_t
);


bool
GetSignature(
  SHA256ProbePtr,
  std::string&
);


bool
GetSignature128(
  SHA256ProbePtr,
  std::string&
);


} // namespace Artifacts

