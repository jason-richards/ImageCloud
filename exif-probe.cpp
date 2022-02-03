#include <cstdlib>
#include <stdexcept>

#include "exif-probe.hpp"

namespace Artifacts {


ProbeExif::ProbeExif(
  const uint8_t * data,
  size_t size
) {
  if (!data || !size) {
    throw std::runtime_error("Empty data field.");
  }

  m_ED = exif_data_new_from_data(data, size);
  if (!m_ED) {
    throw std::runtime_error("EXIF not found.");
  }

}


ProbeExif::~ProbeExif() {
  if (m_ED) {
    exif_data_unref(m_ED);
  }
}


bool
ProbeExif::GetCompression(
  std::string& value
) {
  if (!ReadTag(m_ED, EXIF_IFD_0, EXIF_TAG_COMPRESSION, value) &&
      !ReadTag(m_ED, EXIF_IFD_1, EXIF_TAG_COMPRESSION, value))
  {
    return false;
  }


  return true;
}


bool
ProbeExif::GetWidth(
  int64_t& width
) {
  std::string value;
  if (!ReadTag(m_ED, EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH, value) &&
      !ReadTag(m_ED, EXIF_IFD_1, EXIF_TAG_IMAGE_WIDTH, value))
  {
    return false;
  }

  width = std::strtoull(value.c_str(), nullptr, 0);

  return true;
}


bool
ProbeExif::GetHeight(
  int64_t& height
) {
  std::string value;
  if (!ReadTag(m_ED, EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH, value) &&
      !ReadTag(m_ED, EXIF_IFD_1, EXIF_TAG_IMAGE_LENGTH, value))
  {
    return false;
  }

  height = std::strtoull(value.c_str(), nullptr, 0);

  return true;
}


bool
ProbeExif::GetDateTime(
  std::string& value
) {
  if (!ReadTag(m_ED, EXIF_IFD_0, EXIF_TAG_DATE_TIME, value) &&
      !ReadTag(m_ED, EXIF_IFD_1, EXIF_TAG_DATE_TIME, value))
  {
    return false;
  }


  return true;
}


bool
ProbeExif::GetLatitude(
  std::string& value
) {
  return ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_LATITUDE), value);
}


bool
ProbeExif::GetLongitude(
  std::string& value
) {
  return ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_LONGITUDE), value);
}


bool
ProbeExif::GetAltitude(
  int64_t& altitude
) {
  std::string value;
  if (!ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_ALTITUDE), value)) {
    return false;
  }

  altitude = std::strtoull(value.c_str(), nullptr, 0);

  return true;
}


bool
ProbeExif::ReadTag(
  ExifData *d,
  ExifIfd ifd,
  ExifTag tag,
  std::string& value
) {

  ExifEntry *entry = exif_content_get_entry(d->ifd[ifd], tag);
  if (entry) {
    value.resize(1024, 0);
    exif_entry_get_value(entry, value.data(), value.size()-1);
    if (value.at(0)) {
      return true;
    }
  }

  return false;
}



} // namespace Artifacts

