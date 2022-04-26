#include "miso.hpp"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <streambuf>

namespace Miso {


class Miso {
  rapidjson::Document m_Doc;

  struct membuf : public std::streambuf {
    membuf(char * begin, char * end) {
      this->setg(begin, begin, end);
    }
  };

public:

  Miso() {
    m_Doc.SetObject();

    rapidjson::Document::AllocatorType& allocator = m_Doc.GetAllocator();

    m_Doc.AddMember("uuid",         1, allocator);
    m_Doc.AddMember("signature",    2, allocator);
    m_Doc.AddMember("timestamp",    3, allocator);
    m_Doc.AddMember("compression",  4, allocator);
    m_Doc.AddMember("width",        5, allocator);
    m_Doc.AddMember("height",       6, allocator);
    m_Doc.AddMember("location",     7, allocator);
  }


  Miso(
    char * data,
    size_t size
  ) {
    membuf sbuf(data, data+size);
    std::istream in(&sbuf);
    Parse(in);
  }


  Miso(
    const std::string& jsonPath
  ) {
    std::ifstream ifs(jsonPath);
    if (!ifs.is_open()) {
      throw std::runtime_error("Invalid JSON.");
    }

    Parse(ifs);

    ifs.close();
  }


 ~Miso() {}


  void
  Parse(
    std::istream& ifs
  ) {
    rapidjson::IStreamWrapper isw {ifs};
    rapidjson::ParseResult ok = m_Doc.ParseStream(isw);
    if (m_Doc.HasParseError()) {
      throw std::runtime_error(GetParseError_En(ok.Code()));
    }

    if (!m_Doc.HasMember("uuid") || !m_Doc["uuid"].IsString()) {
      throw std::runtime_error("Missing required UUID.");
    }

    if (!m_Doc.HasMember("signature") || !m_Doc["signature"].IsString()) {
      throw std::runtime_error("Missing required SHA256 signature.");
    }

    if (!m_Doc.HasMember("timestamp") || !m_Doc["timestamp"].IsString()) {
      throw std::runtime_error("Missing required timestamp.");
    }

    if (!m_Doc.HasMember("compression") || !m_Doc["compression"].IsString()) {
      throw std::runtime_error("Missing required compression foramt.");
    }

    if (!m_Doc.HasMember("width") || !m_Doc["width"].IsInt()) {
      throw std::runtime_error("Missing required width.");
    }

    if (!m_Doc.HasMember("height") || !m_Doc["height"].IsInt()) {
      throw std::runtime_error("Missing required height.");
    }

    if (!m_Doc.HasMember("location") || !m_Doc["location"].IsString()) {
      throw std::runtime_error("Missing required location.");
    }
  }


  void
  SetUUID(
    const std::string& uuid
  ) {
    m_Doc["uuid"].SetString(uuid.data(), uuid.size());
  }


  void
  GetUUID(
    std::string& uuid
  ) {
    uuid = m_Doc["uuid"].GetString(); 
  }


  void
  SetSignature(
    const std::string& signature
  ) {
    m_Doc["signature"].SetString(signature.data(), signature.size());
  }

  void
  GetSignature(
    std::string& signature
  ) {
    signature = m_Doc["signature"].GetString(); 
  }


  void
  SetTimeStamp(
    const std::string& ts 
  ) {
    m_Doc["timestamp"].SetString(ts.data(), ts.size());
  }


  void
  GetTimeStamp(
    std::string& ts 
  ) {
    ts = m_Doc["timestamp"].GetString(); 
  }


  void
  SetCompression(
    const std::string& compression
  ) {
    m_Doc["compression"].SetString(compression.data(), compression.size());
  }


  void
  GetCompression(
    std::string& compression
  ) {
    compression = m_Doc["compression"].GetString(); 
  }


  void
  SetWidth(
    int width
  ) {
    m_Doc["width"].SetInt(width);
  }


  void
  GetWidth(
    int& width
  ) {
    width = m_Doc["width"].GetInt();
  }


  void
  SetHeight(
    int height
  ) {
    m_Doc["height"].SetInt(height);
  }


  void
  GetHeight(
    int& height
  ) {
    height = m_Doc["height"].GetInt();
  }


  void
  SetLocation(
    const std::string& location
  ) {
    m_Doc["location"].SetString(location.data(), location.size());
  }


  void
  GetLocation(
    std::string& location
  ) {
    location = m_Doc["location"].GetString();
  }


  void
  SetFaceRectangles(
    const std::vector<FaceRectangleT>& faces
  ) {
    rapidjson::Document::AllocatorType& allocator = m_Doc.GetAllocator();
    rapidjson::Value array(rapidjson::kArrayType);
    for (auto face : faces) {
      rapidjson::Value v;
      v.SetObject();
      v.AddMember("x", face.x, allocator);
      v.AddMember("y", face.y, allocator);
      v.AddMember("width", face.width, allocator);
      v.AddMember("height", face.height, allocator);
      array.PushBack(v, allocator);
    }

    m_Doc.AddMember("faces", array, allocator);
  }


  void
  GetFaceRectangles(
    std::vector<FaceRectangleT>& faces
  ) {
    if (m_Doc.HasMember("faces") && m_Doc["faces"].IsArray()) {
      const auto& f = m_Doc["faces"];
      for (auto iter = f.Begin(); iter != f.End(); iter++) {
        faces.push_back(
          FaceRectangleT{
            .x=(*iter)["x"].GetInt(),
            .y=(*iter)["y"].GetInt(),
            .width=(*iter)["width"].GetInt(),
            .height=(*iter)["height"].GetInt(),
          }
        );
      }
    }
  }


  void
  SetFaceLabels(
    const std::vector<std::string>& labels
  ) {
    rapidjson::Document::AllocatorType& allocator = m_Doc.GetAllocator();
    rapidjson::Value array(rapidjson::kArrayType);
    for (auto label : labels) {
      rapidjson::Value v;
      v.SetString(label.c_str(), label.length(), allocator);
      array.PushBack(v, allocator);
    }

    m_Doc.AddMember("labels", array, allocator);
  }


  void
  GetFaceLabels(
    std::vector<std::string>& labels
  ) {
    if (m_Doc.HasMember("labels") && m_Doc["labels"].IsArray()) {
      const auto& f = m_Doc["labels"];
      for (auto iter = f.Begin(); iter != f.End(); iter++) {
        labels.push_back((*iter).GetString());
      }
    }
  }


  void
  Write(
    std::ostream& out
  ) {
    rapidjson::OStreamWrapper osw {out};
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {osw};
    m_Doc.Accept(writer);
  }
};



MisoPtr
CreateContext() {
  return std::make_shared<Miso>();
}


MisoPtr
CreateContext(
  const std::string& jsonPath
) {
  return std::make_shared<Miso>(jsonPath);
}


MisoPtr
CreateContext(
  char * data,
  size_t size
) {
  return std::make_shared<Miso>(data, size);
}


void
SetUUID(
  MisoPtr context,
  const std::string& uuid
) {
  context->SetUUID(uuid);
}


void
GetUUID(
  MisoPtr context,
  std::string& uuid
) {
  context->GetUUID(uuid);
}


void
SetSignature(
  MisoPtr context,
  const std::string& signature
) {
  context->SetSignature(signature);
}


void
GetSignature(
  MisoPtr context,
  std::string& signature
) {
  context->GetSignature(signature);
}


void
SetTimeStamp(
  MisoPtr context,
  const std::string& ts 
) {
  context->SetTimeStamp(ts);
}


void
GetTimeStamp(
  MisoPtr context,
  std::string& ts 
) {
  context->GetTimeStamp(ts);
}


void
SetCompression(
  MisoPtr context,
  const std::string& compression
) {
  context->SetCompression(compression);
}


void
GetCompression(
  MisoPtr context,
  std::string& compression
) {
  context->GetCompression(compression);
}


void
SetWidth(
  MisoPtr context,
  int width
) {
  context->SetWidth(width);
}


void
GetWidth(
  MisoPtr context,
  int& width
) {
  context->GetWidth(width);
}


void
SetHeight(
  MisoPtr context,
  int height
) {
  context->SetHeight(height);
}


void
GetHeight(
  MisoPtr context,
  int& height
) {
  context->GetHeight(height);
}


void
SetLocation(
  MisoPtr context,
  const std::string& location
) {
  context->SetLocation(location);
}


void
GetLocation(
  MisoPtr context,
  std::string& location
) {
  context->GetLocation(location);
}


void
SetFaceRectangles(
  MisoPtr context,
  const std::vector<FaceRectangleT>& faces
) {
  context->SetFaceRectangles(faces);
}


void
GetFaceRectangles(
  MisoPtr context,
  std::vector<FaceRectangleT>& faces
) {
  context->GetFaceRectangles(faces);
}


void
SetFaceLabels(
  MisoPtr context,
  const std::vector<std::string>& labels
) {
  context->SetFaceLabels(labels);
}


void
GetFaceLabels(
  MisoPtr context,
  std::vector<std::string>& labels
) {
  context->GetFaceLabels(labels);
}


void
Write(
  MisoPtr context,
  std::ostream& out
) {
  context->Write(out);
}


} // namespace Miso

