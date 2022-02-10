#include "media-probe.hpp"

#define _UNICODE
#include <ZenLib/Ztring.h>
#include <MediaInfo/MediaInfo.h>
#include <MediaInfo/MediaInfo_Events.h>

#define DEFAULT_BLOCK_SIZE 7*188

namespace Artifacts {
namespace Media {

class Probe {
    MediaInfoLib::MediaInfo m_MI;
public:

  Probe(
    const uint8_t * data,
    size_t size
  ) {
    size_t position = 0;
    size_t block_sz = DEFAULT_BLOCK_SIZE;

    if (size == 0) {
      return;
    }

    m_MI.Open_Buffer_Init(size, 0);

    do {
      if (position + block_sz > size) {
        break;
      }

      size_t Status = m_MI.Open_Buffer_Continue(data+position, block_sz);
      if (Status & 0x08) {
        break;
      }

      position += block_sz;

      if (m_MI.Open_Buffer_Continue_GoTo_Get()!=(MediaInfo_int64u)-1) {
        position = m_MI.Open_Buffer_Continue_GoTo_Get();
        m_MI.Open_Buffer_Init(size, position);
      }
    } while (position + block_sz < size);

    m_MI.Open_Buffer_Finalize();
  }



  bool
  GetFormat(
    std::string& value
  ) {
    try {
      MediaInfoLib::String s = m_MI.Get(MediaInfoLib::Stream_General, 0, __T("Format"));
      ZenLib::Ztring zenlibZtring(s);
      value = zenlibZtring.To_UTF8();
    } catch (...) {
      return false;
    }
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
GetFormat(
  ProbePtr PP,
  std::string& value
) {
  return PP->GetFormat(value);
}

} // namespace Media
} // namespace Artifacts

