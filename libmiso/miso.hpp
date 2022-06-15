#pragma once

#include <memory>
#include <string>
#include <vector>
#include <ostream>

namespace Miso {

/**************
 *  0-------X
 *  |
 *  |  []
 *  Y
 **************/
typedef struct
ObjectRectangle {
  int x;
  int y;
  int width;
  int height;
} ObjectRectangleT;


/*
  EXAMPLE Manifest JSON:

  {
    "UUID": "1A80E667-8554-11EC-B4CF-D93A08136CE5",
    "signature": "0BE4524E3CE0BDCC7B9C72F9C510CE08",
    "timestamp": "2022:01:08 13:51:53.12",
    "compression" : "JPEG Compression", 
    "width": 3264,
    "height": 1836,
    "latitude": "47 deg 52' 0.25\"N ",
    "longitude": "122 deg 12' 59.37\"W ",
    "faces": [{
      "x": 649,
      "y": 215,
      "width": 20,
      "height": 20
    }, {
      "x": 249,
      "y": 115,
      "width": 30,
      "height": 30
    }]
  }

 */

class Miso;
using MisoPtr = std::shared_ptr<Miso>;

MisoPtr CreateContext   ();
MisoPtr CreateContext   (const std::string& jsonPath);
MisoPtr CreateContext   (char * data, size_t size);

void SetUUID            (MisoPtr context, const std::string& uuid);
void GetUUID            (MisoPtr context, std::string& uuid);
void SetSignature       (MisoPtr context, const std::string& signature);
void GetSignature       (MisoPtr context, std::string& signature);
void SetTimeStamp       (MisoPtr context, const std::string& ts);
void GetTimeStamp       (MisoPtr context, std::string& ts);
void SetCompression     (MisoPtr context, const std::string& compression);
void GetCompression     (MisoPtr context, std::string& compression);
void SetWidth           (MisoPtr context, int width);
void GetWidth           (MisoPtr context, int& width);
void SetHeight          (MisoPtr context, int height);
void GetHeight          (MisoPtr context, int& height);
void SetLocation        (MisoPtr context, const std::string& location);
void GetLocation        (MisoPtr context, std::string& location);
void SetObjectRectangles(MisoPtr context, const std::string& object, const std::vector<ObjectRectangleT>& rects);
void GetObjectRectangles(MisoPtr context, const std::string& object, std::vector<ObjectRectangleT>& rects);
void SetFaceLabels      (MisoPtr context, const std::vector<std::string>& labels);
void GetFaceLabels      (MisoPtr context, std::vector<std::string>& labels);
void Write              (MisoPtr context, std::ostream&);

} // namespace Miso

