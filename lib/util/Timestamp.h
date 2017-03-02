#ifndef OPLIB_TIMESTAMP_H
#define OPLIB_TIMESTAMP_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <sys/time.h>
#include <string>

namespace oplib
{
  class Timestamp
  {
  public:

    Timestamp()
    : _timeInMicroSeconds(0)
    {}

    explicit Timestamp(int64_t microsecs_)
    : _timeInMicroSeconds(microsecs_)
    {}

    time_t seconds() { return _timeInMicroSeconds / numMicroSecondsInSeconds; }

    int64_t microseconds() { return _timeInMicroSeconds; }

    std::string toString() const;

    std::string toFormatString() const;

    static const int64_t numMicroSecondsInSeconds { 1000 * 1000 };

    static Timestamp now();
    static Timestamp epochTime()
    {
      return Timestamp();
    }

    bool valid() const
    {
      return _timeInMicroSeconds != 0;
    }

    static Timestamp unixTime(time_t secs_, int64_t micro_ = 0)
    {
      return Timestamp(static_cast<int64_t>(secs_) * numMicroSecondsInSeconds + micro_);
    }

    static double timeDiff(Timestamp earlier_, Timestamp later_);

    friend bool operator < (Timestamp lhs, Timestamp rhs);
    friend bool operator == (Timestamp lhs, Timestamp rhs);
    friend int64_t operator - (Timestamp lhs, Timestamp rhs);
    Timestamp& operator += (double timediff_);

  private:
    int64_t _timeInMicroSeconds; 
  };

  inline Timestamp Timestamp::now()
  {
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return Timestamp(static_cast<int64_t>(tv.tv_sec) * numMicroSecondsInSeconds
                     + tv.tv_usec);
  }

  inline Timestamp& Timestamp::operator += (double timediff_)
  {
    int64_t timediffMicro = static_cast<int64_t>(timediff_ * numMicroSecondsInSeconds);
    _timeInMicroSeconds += timediffMicro;
    return *this;
  }

  inline double Timestamp::timeDiff(Timestamp earlier_, Timestamp later_)
  {
    int64_t diff = later_ - earlier_;
    return static_cast<double>(diff) / numMicroSecondsInSeconds;
  }

  inline bool operator < (Timestamp lhs, Timestamp rhs)
  {
    return lhs._timeInMicroSeconds < rhs._timeInMicroSeconds;
  }

  inline bool operator == (Timestamp lhs, Timestamp rhs)
  {
    return lhs._timeInMicroSeconds == rhs._timeInMicroSeconds;
  }

  inline int64_t operator - (Timestamp lhs, Timestamp rhs)
  {
    return lhs._timeInMicroSeconds - rhs._timeInMicroSeconds;
  }

  inline std::string Timestamp::toString() const
  {
    char buf[32] = {0};
    int64_t seconds = _timeInMicroSeconds / numMicroSecondsInSeconds;
    int64_t microSeconds = _timeInMicroSeconds % numMicroSecondsInSeconds;
    ::snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microSeconds);
    return buf;
  }

  inline std::string Timestamp::toFormatString() const
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

#endif
