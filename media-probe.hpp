#pragma once

#define _UNICODE
#include <ZenLib/Ztring.h>
#include <MediaInfo/MediaInfo.h>
#include <MediaInfo/MediaInfo_Events.h>

#define DEFAULT_BLOCK_SIZE 7*188

namespace Artifacts {


class ProbeMedia {
public:

  ProbeMedia(
    const uint8_t * data,
    size_t size
  );


  bool
  GetFormat(
    std::string& value
  );

private:

    MediaInfoLib::MediaInfo m_MI;

};

} // namespace Artifacts

