#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Artifacts {
namespace Exif {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;


/* Create a new Exif probe.
 *
 * @param uint8_t * - Pointer to photo data.
 * @param size_t - Image size.
 * @result ProbePtr - Exif probe pointer.
 */
ProbePtr
CreateProbe(
  const uint8_t *,
  size_t
);


/* Get the compression format of the photo.
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


/* Get the width of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting photo width.
 * @result bool - True if successfully found width.
 */
bool
GetWidth(
  ProbePtr,
  int&
);


/* Get the height of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting photo height.
 * @result bool - True if successfully found height.
 */
bool
GetHeight(
  ProbePtr,
  int&
);


/* Get the timestamp of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param int - Resulting photo timestamp.
 * @result bool - True if successfully found timestamp.
 */
bool
GetDateTime(
  ProbePtr,
  std::string&
);


/* Get the Latitude of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting photo Latitude.
 * @result bool - True if successfully found Latitude.
 */
bool
GetLatitude(
  ProbePtr,
  std::string&
);


/* Get the Longitude of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting photo Longitude.
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


/* Get the Altitude of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting photo Altitude.
 * @result bool - True if successfully found Altitude.
 */
bool
GetAltitude(
  ProbePtr,
  int&
);


/* Get the Thumbnail of the photo.
 *
 * @param ProbePtr - Pointer to the Exif probe object.
 * @param std::vector - Resulting photo thumbnail.
 * @result bool - True if successfully found Thumbnail.
 */
bool
GetThumbnail(
  ProbePtr,
  std::vector<char>&
);

} // namespace Exif
} // namespace Artifacts

