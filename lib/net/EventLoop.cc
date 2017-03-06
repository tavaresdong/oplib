#include "EventLoop.h"
#include "EventDispatcher.h"
#include "Poller.h"
#include "TimerManager.h"

#include <util/Timestamp.h>

#include <assert.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <algorithm>

namespace oplib
{

  // Thread local pointer: every thread can only 
  // have one loop
  __thread EventLoop* gLoopInThread { nullptr };

  EventLoop::EventLoop()
  : _threadId(CurrentThread::tid()),
    _poller(std::make_unique<Poller>(this)),
    _timerMgr(std::make_unique<TimerManager>(this)),
    _wakeupfd( ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
    _wakeupDispatcher(std::make_unique<EventDispatcher>(this, _wakeupfd))
  {
    if (gLoopInThread != nullptr)
    {
      // TODO: add log here
      abort();
    }
    gLoopInThread = this;

    // When this eventfd is readable: execute all pending functors
    // bind: take the address of a function
    _wakeupDispatcher->setReadCallback(
      std::bind(&EventLoop::handleRead, this));
    _wakeupDispatcher->enableReading();
  }

  void EventLoop::inLoopThreadOrDie()
  {
    if (!inLoopThread())
    {
      // TODO: add log here
      abort();
    } 
  }

  EventLoop::~EventLoop()
  {
    // If the loop is still looping, it cannot be destroyed
    assert(!_looping);
    gLoopInThread = nullptr;
  }

  void EventLoop::updateEventDispatcher(EventDispatcher* dp_)
  {
    _poller->updateEventDispatcher(dp_);
  }

  void EventLoop::loop()
  {
    // Only called from the loop thread
    // and when not looping
    assert(inLoopThread());
    assert(!_looping);
    Poller::EventDispatcherList activeDispatchers;
    _looping.exchange(true);

    while (!_done)
    {
      activeDispatchers.clear();
      _poller->poll(_timeout, &activeDispatchers);

      for (auto dispatcher : activeDispatchers)
      {
        // When the events are filled, 
        // call handleEvent on all the dispatchers
        dispatcher->handleEvent();
      }

      // Execute pending functors here
      executePendingFunctors();
    }

    _looping.exchange(false);
  }

  void EventLoop::runAt(const Timestamp& when_, const TimerCallback& cb_)
  {
    _timerMgr->addTimer(cb_, when_, 0.0);
  }

  void EventLoop::runEvery(double interval_, const TimerCallback& cb_)
  {
    Timestamp fire { Timestamp::now() };
    fire += interval_;
    _timerMgr->addTimer(cb_, fire, interval_);
  }

  void EventLoop::runAfter(double delay_, const TimerCallback& cb_)
  {
    Timestamp fire { Timestamp::now() };
    fire += delay_; 
    _timerMgr->addTimer(cb_, fire, 0.0);
  }

  void EventLoop::runInLoop(const Functor& func_)
  {
    // If in loop thread, call this functor directly
    if (inLoopThread())
      func_();
    else
    {
      enqueue(func_);
    }
  }

  void EventLoop::enqueue(const Functor& func_)
  {
    {
      oplib::MutexLockGuard guard(_mutex);
      _pendingFunctors.push_back(func_);
    }

    // (1) currently not in loop thread, must wake up or the functor
    // won't get executed in time
    // (2) Currently executing functors, need to wakeup
    // so the functors newly added could get executed in time
    if (!inLoopThread() || _executingFunctors)
    {
      wakeup();
    }
  }

  void EventLoop::handleRead()
  {
    // Read the eventfd : we are level-trigger
    // Sucessful read will return an 8-byte integer
    uint64_t buf;
    ssize_t numRead = ::read(_wakeupfd, &buf, sizeof(buf));
    assert(numRead == sizeof(buf));
  }

  void EventLoop::executePendingFunctors()
  {
    _executingFunctors.exchange(true);
    std::vector<Functor> toExecute;
    {
      // Minimize the critical section
      MutexLockGuard guard(_mutex);
      toExecute.swap(_pendingFunctors);
    }

    // Execute the functors sequentially
    for (auto& func : toExecute)
    {
      func();
    }

    _executingFunctors.exchange(false);
  }

  void EventLoop::wakeup()
  {
    uint64_t buffer = 1;
    ssize_t numWrite = ::write(_wakeupfd, &buffer, sizeof(buffer));  

    assert(numWrite == sizeof(buffer));
  }
}
