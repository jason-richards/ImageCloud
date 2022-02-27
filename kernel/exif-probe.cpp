#include "exif-probe.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <libexif/exif-ifd.h>
#include <libexif/exif-data.h>


namespace Artifacts {
namespace Exif {

class Probe {
public:

  Probe(
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


  ~Probe() {
    if (m_ED) {
      exif_data_unref(m_ED);
    }
  }


  bool
  GetCompression(
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
  GetWidth(
    std::string& value
  ) {
    if (!ReadTag(m_ED, EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH, value) &&
        !ReadTag(m_ED, EXIF_IFD_1, EXIF_TAG_IMAGE_WIDTH, value))
    {
      return false;
    }

    return true;
  }


  bool
  GetHeight(
    std::string& value
  ) {
    if (!ReadTag(m_ED, EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH, value) &&
        !ReadTag(m_ED, EXIF_IFD_1, EXIF_TAG_IMAGE_LENGTH, value))
    {
      return false;
    }

    return true;
  }


  bool
  GetDateTime(
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
  GetLatitude(
    std::string& value
  ) {
    return ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_LATITUDE), value);
  }


  bool
  GetLongitude(
    std::string& value
  ) {
    return ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_LONGITUDE), value);
  }


  bool
  ToDegrees(
    const std::string& DMS,
    float& DD
  ) {
    float degrees = 0, minutes = 0, seconds = 0;
    if (std::sscanf(DMS.c_str(), "%f, %f, %f", &degrees, &minutes, &seconds) != 3) {
      return false;
    }

    DD = degrees + (minutes / 60.0) + (seconds / 3600.0);

    return true;
  }


  bool
  GetLocation(
    float& latitude,
    float& longitude
  ) {
    std::string value;
    if (!GetLatitude(value) || !ToDegrees(value, latitude)) {
      return false;
    }

    if (!ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_LATITUDE_REF), value)) {
      return false;
    }

    if (value.compare("N")) {
      latitude = 0 - latitude;
    }

    if (!GetLongitude(value) || !ToDegrees(value, longitude)) {
      return false;
    }

    if (!ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_LONGITUDE_REF), value)) {
      return false;
    }

    if (value.compare("E")) {
      longitude = 0 - longitude;
    }

    return true;
  }


  bool
  GetAltitude(
    int& altitude
  ) {
    std::string value;
    if (!ReadTag(m_ED, EXIF_IFD_GPS, static_cast<ExifTag>(EXIF_TAG_GPS_ALTITUDE), value)) {
      return false;
    }

    altitude = std::stoi(value.c_str(), nullptr, 0);

    return true;
  }
private:

  bool
  ReadTag(
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
        value.erase(value.find('\0'));
        return true;
      }
    }

    return false;
  }

  ExifData * m_ED;
};


/* Create a new Exif probe.
 *
 * @param uint8_t * - Pointer to image data.
 * @param size_t - Image size.
 * @result ProbePtr - Exif probe pointer.
 */
ProbePtr
CreateProbe(
  const uint8_t * data,
  size_t size
) {
  return std::make_shared<Probe>(data, size);
}


/* Get the compression format of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting compression format string
 * @result bool - True if successfully found compression format.
 */
bool
GetCompression(
  ProbePtr EP,
  std::string& value
) {
  return EP->GetCompression(value);
}


/* Get the width of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param int - Resulting image width.
 * @result bool - True if successfully found width.
 */
bool
GetWidth(
  ProbePtr EP,
  int& value
) {
  std::string s;
  bool result = EP->GetWidth(s);
  if (result) {
    value = std::stoi(s.c_str(), nullptr, 0);
  }
  return result;
}

bool
GetWidth(
  ProbePtr EP,
  std::string& value
) {
  return EP->GetWidth(value);
}


/* Get the height of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param int - Resulting image height.
 * @result bool - True if successfully found height.
 */
bool
GetHeight(
  ProbePtr EP,
  int& value
) {
  std::string s;
  bool result = EP->GetHeight(s);
  if (result) {
    value = std::stoi(s.c_str(), nullptr, 0);
  }

  return result;
}

bool
GetHeight(
  ProbePtr EP,
  std::string& value
) {
  return EP->GetHeight(value);
}


/* Get the timestamp of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param int - Resulting image timestamp.
 * @result bool - True if successfully found timestamp.
 */
bool
GetDateTime(
  ProbePtr EP,
  std::string& value
) {
  return EP->GetDateTime(value);
}


/* Get the Latitude of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Latitude.
 * @result bool - True if successfully found Latitude.
 */
bool
GetLatitude(
  ProbePtr EP,
  std::string& value
) {
  return EP->GetLatitude(value);
}


/* Get the Longitude of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Longitude.
 * @result bool - True if successfully found Longitude.
 */
bool
GetLongitude(
  ProbePtr EP,
  std::string& value
) {
  return EP->GetLongitude(value);
}

/* Get the GPS Coordinates in Decimal Degree format.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Latitude, Longitude in degrees.
 * @result bool - True if successfully obtained coordinates.
 */
bool
GetLocation(
  ProbePtr EP,
  std::string& value
) {
  float latitude = 0, longitude = 0;

  if (!EP->GetLocation(latitude, longitude)) {
    return false;
  }

  value = std::string(std::to_string(latitude) + ", " + std::to_string(longitude));

  return true;
}


/* Get the Altitude of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Altitude.
 * @result bool - True if successfully found Altitude.
 */
bool
GetAltitude(
  ProbePtr EP,
  int& value
) {
  return EP->GetAltitude(value);
}


} // namespace Exif
} // namespace Artifacts

