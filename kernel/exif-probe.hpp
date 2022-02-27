#pragma once

#include <memory>
#include <string>


namespace Artifacts {
namespace Exif {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;


/* Create a new Exif probe.
 *
 * @param uint8_t * - Pointer to image data.
 * @param size_t - Image size.
 * @result ProbePtr - Exif probe pointer.
 */
ProbePtr
CreateProbe(
  const uint8_t *,
  size_t
);


/* Get the compression format of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting compression format string
 * @result bool - True if successfully found compression format.
 */
bool
GetCompression(
  ProbePtr,
  std::string&
);


/* Get the width of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image width.
 * @result bool - True if successfully found width.
 */
bool
GetWidth(
  ProbePtr,
  int&
);


/* Get the height of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image height.
 * @result bool - True if successfully found height.
 */
bool
GetHeight(
  ProbePtr,
  int&
);


/* Get the timestamp of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param int - Resulting image timestamp.
 * @result bool - True if successfully found timestamp.
 */
bool
GetDateTime(
  ProbePtr,
  std::string&
);


/* Get the Latitude of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Latitude.
 * @result bool - True if successfully found Latitude.
 */
bool
GetLatitude(
  ProbePtr,
  std::string&
);


/* Get the Longitude of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Longitude.
 * @result bool - True if successfully found Longitude.
 */
bool
GetLongitude(
  ProbePtr,
  std::string&
);


/* Get the GPS Coordinates in Decimal Degree format.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Latitude, Longitude in degrees.
 * @result bool - True if successfully obtained coordinates.
 */
bool
GetLocation(
  ProbePtr,
  std::string&
);


/* Get the Altitude of the image.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Altitude.
 * @result bool - True if successfully found Altitude.
 */
bool
GetAltitude(
  ProbePtr,
  int&
);


} // namespace Exif
} // namespace Artifacts

