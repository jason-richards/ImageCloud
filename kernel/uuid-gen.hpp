#pragma once

#include <uuid/uuid.h>


class UUIDGenerator {
  uuid_t m_UUID;
public:
  UUIDGenerator() { uuid_generate_time_safe(m_UUID); }

  std::string operator()() const {
    std::string uuidString(36, 0);
    uuid_unparse_upper(m_UUID, uuidString.data());
    return uuidString;
  }

  std::string
  GetID() {
    return (*this)();
  }
};


