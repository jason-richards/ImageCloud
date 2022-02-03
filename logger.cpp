#include "logger.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace Logging {


void
logger(
  const char* tag,
  const char* message
) {
   time_t now;
   time(&now);
   printf("%s [%s]: %s\n", ctime(&now), tag, message);
}


} // namespace Logging

