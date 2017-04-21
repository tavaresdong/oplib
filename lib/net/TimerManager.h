#ifndef OPLIB_TIMERMANAGER_H
#define OPLIB_TIMERMANAGER_H

#include <util/Common.h>
#include <util/Timestamp.h>
#include "EventDispatcher.h"

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <atomic>

namespace oplib
{
  class Timer : Noncopyable
  {
   public:
    Timer(TimerCallback cb_, Timestamp when_, double interval_)
    : _timerCallback(cb_),
      _when(when_),
      _interval(interval_), 
      _repeating(interval_ > 0.0),
      _id(++_sequence)
    {}

    void run() { _timerCallback(); }

    int id() { return _id; }

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
    int _id;

    // Atomic incrementing sequence number for timers created
    // to identify different timers because their address could be the same
    static std::atomic<int> _sequence;
  };

  typedef std::pair<std::shared_ptr<Timer>, int> TimerId;

  class TimerManager : Noncopyable
  {
   public:

    using TimerList = std::vector<std::shared_ptr<Timer>>;

    TimerManager(EventLoop* loop_);
    ~TimerManager();

    void handleRead();
    
    TimerList expiredTimers(Timestamp current_);

    TimerId addTimer(const TimerCallback& cb_, Timestamp when_, double interval_);

    void cancel(const TimerId& timerId_);

   private:
    using TimerMap = std::multimap<Timestamp, std::shared_ptr<Timer>>;
    using ActiveTimerSet = std::set<TimerId>;

    void addTimerInLoop(const std::shared_ptr<Timer>& timer_);
    void cancelInLoop(const TimerId& timerId_);

    void reset(const TimerList& timerlist_);
    bool insert(const std::shared_ptr<Timer>& timerptr_);

    // Reset timerfd to watch the most recently triggered timer in the future
    void resetTimerfd();

    // TODO: implement binary head/4-heap to store non-expired timers

    EventLoop* _loop;
    const int _timerfd;
    EventDispatcher _dispatcher;
    TimerMap _timers;
    ActiveTimerSet _activeTimers;
    ActiveTimerSet _cancelledTimers;
    bool _executingExpiredTimers;
  };

}

#endif
