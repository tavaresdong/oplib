#ifndef OPLIB_TIMERMANAGER_H
#define OPLIB_TIMERMANAGER_H

#include <util/Common.h>
#include <util/Timestamp.h>
#include "EventDispatcher.h"

#include <vector>
#include <map>
#include <memory>

namespace oplib
{
  class Timer : Noncopyable
  {
   public:
    Timer(TimerCallback cb_, Timestamp when_, double interval_)
    : _timerCallback(cb_), _when(when_), _interval(interval_), _repeating(interval_ > 0.0)
    {}

    void run() { _timerCallback(); }

    Timestamp expireTime() const { return _when; }
    bool repeating() const { return _repeating; }

    void restart(Timestamp from_)
    {
      if (_repeating)
      {
        _when = from_;
        _when += _interval;
      }
      else
      {
        _when = Timestamp::epochTime();
      }
    }

   private:
    TimerCallback _timerCallback;
    Timestamp _when;
    double _interval;
    bool _repeating;
  };

  // Store the address of a timer
  // struct TimerId
  // {
  //   TimerId(Timer* addr_) : _addr(addr_) {}
  //   Timer* _addr;
  // };

  class TimerManager : Noncopyable
  {
   public:

    using TimerList = std::vector<std::shared_ptr<Timer>>;

    TimerManager(EventLoop* loop_);
    ~TimerManager();

    void handleRead();
    
    TimerList expiredTimers(Timestamp current_);

    void addTimer(const TimerCallback& cb_, Timestamp when_, double interval_);

    // TODO cancel a timer
   private:
    using TimerMap = std::multimap<Timestamp, std::shared_ptr<Timer>>;

    void reset(const TimerList& timerlist_);

    // Reset timerfd to watch the most recently triggered timer in the future
    void resetTimerfd();

    // Insert a timer into the TimerManager, update timerfd if needed
    bool insert(const std::shared_ptr<Timer>& timerptr_);

    // TODO: implement binary head/4-heap to store non-expired timers

    EventLoop* _loop;
    const int _timerfd;
    EventDispatcher _dispatcher;
    TimerMap _timers;
  };

}

#endif
