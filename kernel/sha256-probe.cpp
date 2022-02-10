#include "sha256-probe.hpp"
#include <vector>
#include <algorithm>
#include <openssl/sha.h>

namespace Artifacts {
namespace SHA256 {

class Probe {
  std::vector<uint8_t> m_Signature;
public: 


  Probe(
    const uint8_t * data,
    size_t size
  ) {
    m_Signature.resize(SHA256_DIGEST_LENGTH, 0);
    SHA256_CTX sha256ctx;
    SHA256_Init(&sha256ctx);
    SHA256_Update(&sha256ctx, data, size);
    SHA256_Final(m_Signature.data(), &sha256ctx);
  }


  bool
  GetSignature(
    std::string& value
  ) {
    std::for_each(m_Signature.begin(), m_Signature.end(),
      [&value](uint8_t b) {
        uint8_t n0 = ((b>>4)&0xF);
        uint8_t n1 = ((b)   &0xF);
        value.push_back(n0 + (n0 < 10 ? '0' : '7')); 
        value.push_back(n1 + (n1 < 10 ? '0' : '7')); 
      }
    );

    return true;
  }

};


ProbePtr
CreateProbe(
  const uint8_t * data,
  size_t size
) {
  return std::make_shared<Probe>(data, size);
}


bool
GetSignature(
  ProbePtr probe,
  std::string& value
) {
  return probe->GetSignature(value);
}


bool
GetSignature128(
  ProbePtr probe,
  std::string& value
) {
  bool result = probe->GetSignature(value);
  if (result) {
    value.resize(32);
  }
  return result;
}

} // namespace SHA256
} // namespace Artifacts

