#include "Timestamp.h"

namespace oplib
{
  std::string Timestamp::toString() const
  {
    char buf[32] = {0};
    int64_t seconds = _timeInMicroSeconds / numMicroSecondsInSeconds;
    int64_t microSeconds = _timeInMicroSeconds % numMicroSecondsInSeconds;
    ::snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microSeconds);
    return buf;
  }

  std::string Timestamp::toFormatString() const
  {
    char buf[32] = {0};
    int64_t seconds = _timeInMicroSeconds / numMicroSecondsInSeconds;
    int microSeconds = _timeInMicroSeconds % numMicroSecondsInSeconds;

    struct tm tmval;
    ::gmtime_r(&seconds, &tmval);

    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d:%06d", 
             tmval.tm_year, tmval.tm_mon, tmval.tm_mday,
             tmval.tm_hour, tmval.tm_min, tmval.tm_sec, microSeconds);
    return buf;
  }
}
