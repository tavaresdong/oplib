#include "TimerManager.h"
#include "EventLoop.h"

#include <cassert>
#include <unistd.h>
#include <sys/timerfd.h>
#include <strings.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <functional>

namespace oplib
{
  std::atomic<int> Timer::_sequence { 0 };

  namespace timerfd
  {
    int createTimerfd()
    {
      int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
      if (timerfd < 0)
      {
        // TODO if timerfd < 0 log error and abort
        printf("create timerfd error\n");
        abort();
      }
      return timerfd;
    }
  }

  TimerManager::TimerManager(EventLoop* loop_)
  : _loop(loop_), _timerfd(timerfd::createTimerfd()), _dispatcher(loop_, _timerfd)
  {
    // Callback called by loop is the handleRead() method defined in TimerManager
    _dispatcher.setReadCallback(std::bind(&TimerManager::handleRead, this));
    _dispatcher.enableReading();
  }

  TimerManager::~TimerManager()
  {
    ::close(_timerfd);
  }

  TimerManager::TimerList TimerManager::expiredTimers(Timestamp current_)
  {
    assert(_timers.size() == _activeTimers.size());
    std::vector<std::shared_ptr<Timer>> expireds;
    auto iter = _timers.lower_bound(current_);
    assert(iter == _timers.end() || current_ < iter->first);
    std::for_each(_timers.begin(), iter, [ &expireds ](auto& kv) {
              expireds.push_back(kv.second);
            });
    _timers.erase(_timers.begin(), iter);
    std::for_each(expireds.begin(), expireds.end(), [ this ](auto& ptr) {
              _activeTimers.erase(TimerId(ptr, ptr->id()));
            });

    assert(_timers.size() == _activeTimers.size());
    return expireds;
  }

  void TimerManager::handleRead()
  {
    // When this function is called, remove all expired timers and 
    // cann their run() method

    // Level trigger, we need to read _timerfd
    uint64_t timeoutCount;
    ssize_t numRead = ::read(_timerfd, &timeoutCount, sizeof(timeoutCount));

    auto expireds = expiredTimers(Timestamp::now());

    // When executing timers, cancelInLoop could be called to cancel
    // some timers which is being executed(in expireds)
    // we need to mark these and not resetting them
    _executingExpiredTimers = true;
    _cancelledTimers.clear();

    std::for_each(expireds.begin(), expireds.end(),
                  [](auto& timerp) {
                    timerp->run();
                  });
    _executingExpiredTimers = false;

    reset(expireds);
  }

  void TimerManager::reset(const TimerList& timerlist_)
  {
    for (auto& timerptr : timerlist_)
    {
      auto iter = _cancelledTimers.find(TimerId(timerptr, timerptr->id()));
      if (timerptr->repeating() && iter == _cancelledTimers.end())
      {
        timerptr->restart(Timestamp::now());
        insert(timerptr);
      }
    }

    if (!_timers.empty())
    {
      resetTimerfd();
    }
  }

  bool TimerManager::insert(const std::shared_ptr<Timer>& timerptr_)
  {
    assert(_timers.size() == _activeTimers.size());
    bool firstExpireChanged = false;
    auto first = _timers.begin();
    if (first == _timers.end() || timerptr_->expireTime() < first->second->expireTime())
    {
      firstExpireChanged = true;
    }

    // Insert into a multimap always succeeds
    _timers.insert(std::pair<Timestamp, std::shared_ptr<Timer>>
            (timerptr_->expireTime(), timerptr_));
    _activeTimers.insert(TimerId(timerptr_, timerptr_->id()));

    assert(_timers.size() == _activeTimers.size());
    return firstExpireChanged;
  }

  void TimerManager::resetTimerfd()
  {
    assert(!_timers.empty());
    Timestamp expire = _timers.begin()->first;

    // Must be a valid time in the future
    if (!expire.valid()) return;
    
    struct itimerspec oldValue;
    struct itimerspec newValue;
    bzero(&oldValue, sizeof(oldValue));
    bzero(&newValue, sizeof(newValue));

    int64_t gap = expire.microseconds() - Timestamp::now().microseconds();
    if (gap < 100) gap = 100;
    newValue.it_value.tv_sec =
        static_cast<time_t>(gap / Timestamp::numMicroSecondsInSeconds);
    newValue.it_value.tv_nsec =
        static_cast<long>((gap % Timestamp::numMicroSecondsInSeconds) * 1000);
    auto ret = ::timerfd_settime(_timerfd, 0, &newValue, &oldValue);
    assert(ret == 0);
  }

  void TimerManager::addTimerInLoop(const std::shared_ptr<Timer>& timer_)
  {
    // Only executed in loop thread
    _loop->inLoopThreadOrDie();
    bool needReset = insert(timer_);
    if (needReset)
    {
      resetTimerfd();
    }
  }

  TimerId TimerManager::addTimer(const TimerCallback& cb_, Timestamp when_, double interval_)
  {
    // This method can only be called from EventLoop
    // TODO: make it more thread-safe_loop->inLoopThreadOrDie();
    auto timerptr = std::make_shared<Timer>(cb_, when_, interval_);
    _loop->runInLoop(std::bind(&TimerManager::addTimerInLoop, this, timerptr)); 
    return TimerId(timerptr, timerptr->id());
  }

  void TimerManager::cancel(const TimerId& timerId_)
  {
    _loop->runInLoop(std::bind(&TimerManager::cancelInLoop, this, timerId_));
  }

  void TimerManager::cancelInLoop(const TimerId& timerId_)
  {
    _loop->inLoopThreadOrDie();
    assert(_timers.size() == _activeTimers.size());
    auto iter = _activeTimers.find(timerId_);
    if (iter != _activeTimers.end())
    {
      Timestamp ts = timerId_.first->expireTime();
      auto range = _timers.equal_range(ts);
      auto it = range.first;
      for (auto it = range.first; it != range.second; ++it)
      {
        if (it->second->id() == timerId_.second)
          break;
      }
      assert(it != range.second);
      _timers.erase(it);
      _activeTimers.erase(iter);
      assert(_timers.size() == _activeTimers.size());

      printf("Timer cancelled\n");
    }
    else if (_executingExpiredTimers)
    {
      // Calling expired timer which asks to cancel itself
      _cancelledTimers.insert(timerId_);
    }
  }
}
