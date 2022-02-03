#pragma once

namespace Logging {

void logger(const char * tag, const char * message);

#define LOG_INFO(message)  logger("INFO",    msg);
#define LOG_WARN(message)  logger("WARNING", msg);
#define LOG_ERROR(message) logger("ERROR",   msg);
#define LOG_DEBUG(message) logger("DEBUG",   msg);

} // namespace Logging

