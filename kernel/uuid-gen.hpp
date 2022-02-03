#pragma once

#include <uuid/uuid.h>


class UUIDGenerator {
  uuid_t m_UUID;
public:
  UUIDGenerator() { uuid_generate_time_safe(m_UUID); }

  std::string operator()() const {
    std::string uuidString(37, 0);
    uuid_unparse_upper(m_UUID, uuidString.data());
    return uuidString;
  }
};


