#pragma once

#include <cstdlib>
#include <stdexcept>

#include <libexif/exif-ifd.h>
#include <libexif/exif-data.h>

namespace Artifacts {


class ProbeExif {
public:


  ProbeExif(
    const uint8_t * data,
    size_t size
  );


 ~ProbeExif();


  bool
  GetCompression(
    std::string& value
  );


  bool
  GetWidth(
    int64_t& width
  );


  bool
  GetHeight(
    int64_t& height
  );


  bool
  GetDateTime(
    std::string& value
  );


  bool
  GetLatitude(
    std::string& value
  );


  bool
  GetLongitude(
    std::string& value
  );


  bool
  GetAltitude(
    int64_t& altitude
  );


private:

  bool
  ReadTag(
    ExifData *d,
    ExifIfd ifd,
    ExifTag tag,
    std::string& value
  );

  ExifData * m_ED;
};



} // namespace Artifacts

