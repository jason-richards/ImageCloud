#pragma once

#include <memory>
#include <string>


namespace Artifacts {


class ExifProbe;
using ExifProbePtr = std::shared_ptr<ExifProbe>;


/* Create a new Exif probe.
 *
 * @param uint8_t * - Pointer to image data.
 * @param size_t - Image size.
 * @result ExifProbePtr - Exif probe pointer.
 */
ExifProbePtr
CreateExifProbe(
  const uint8_t *,
  size_t
);


/* Get the compression format of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting compression format string
 * @result bool - True if successfully found compression format.
 */
bool
GetCompression(
  ExifProbePtr,
  std::string&
);


/* Get the width of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image width.
 * @result bool - True if successfully found width.
 */
bool
GetWidth(
  ExifProbePtr,
  int&
);


/* Get the height of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image height.
 * @result bool - True if successfully found height.
 */
bool
GetHeight(
  ExifProbePtr,
  int&
);


/* Get the timestamp of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param int - Resulting image timestamp.
 * @result bool - True if successfully found timestamp.
 */
bool
GetDateTime(
  ExifProbePtr,
  std::string&
);


/* Get the Latitude of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Latitude.
 * @result bool - True if successfully found Latitude.
 */
bool
GetLatitude(
  ExifProbePtr,
  std::string&
);


/* Get the Longitude of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Longitude.
 * @result bool - True if successfully found Longitude.
 */
bool
GetLongitude(
  ExifProbePtr,
  std::string&
);


/* Get the Altitude of the image.
 *
 * @param ExifProbePtr - Pointer to the Exif probe object.
 * @param std::string - Resulting image Altitude.
 * @result bool - True if successfully found Altitude.
 */
bool
GetAltitude(
  ExifProbePtr,
  int&
);


} // namespace Artifacts

