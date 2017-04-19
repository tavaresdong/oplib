#include "TimerManager.h"
#include "EventLoop.h"

#include <assert.h>
#include <algorithm>
#include <iterator>
#include <functional>

#include <unistd.h>
#include <sys/timerfd.h>
#include <strings.h>
#include <cstdlib>
#include <cstdio>

#include <iostream>

namespace oplib
{
  namespace timerfd
  {
    int createTimerfd()
    {
      int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
      // TODO if timerfd < 0 log error and abort
      if (timerfd < 0)
      {
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
    std::vector<std::shared_ptr<Timer>> expireds;
    auto iter = _timers.lower_bound(current_);
    assert(iter == _timers.end() || current_ < iter->first);
    std::for_each(_timers.begin(), iter, [ &expireds ](auto& kv) {
              expireds.push_back(kv.second);
            });
    _timers.erase(_timers.begin(), iter);

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
    std::for_each(expireds.begin(), expireds.end(),
                  [](auto& timersp) {
                    timersp->run();
                  });

    reset(expireds);
  }

  void TimerManager::reset(const TimerList& timerlist_)
  {
    for (auto& timerptr : timerlist_)
    {
      if (timerptr->repeating())
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
    bool firstExpireChanged = false;
    auto first = _timers.begin();
    if (first == _timers.end() || timerptr_->expireTime() < first->second->expireTime())
    {
      firstExpireChanged = true;
    }

    // Insert into a multimap always succeeds
    _timers.insert(std::pair<Timestamp, std::shared_ptr<Timer>>
            (timerptr_->expireTime(), timerptr_));

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

  void TimerManager::addTimer(const TimerCallback& cb_, Timestamp when_, double interval_)
  {
    // This method can only be called from EventLoop
    // TODO: make it more thread-safe_loop->inLoopThreadOrDie();
    auto timerptr = std::make_shared<Timer>(cb_, when_, interval_);
    _loop->runInLoop(std::bind(&TimerManager::addTimerInLoop, this, timerptr)); 
  }
}
