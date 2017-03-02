#include "EventLoop.h"
#include "EventDispatcher.h"
#include "Poller.h"
#include "TimerManager.h"

#include <util/Timestamp.h>

#include <assert.h>

namespace oplib
{

  // Thread local pointer: every thread can only 
  // have one loop
  __thread EventLoop* gLoopInThread { nullptr };

  EventLoop::EventLoop()
  : _threadId(CurrentThread::tid()),
    _poller(std::make_unique<Poller>(this)),
    _timerMgr(std::make_unique<TimerManager>(this))
  {
    if (gLoopInThread != nullptr)
    {
      // TODO: add log here
      abort();
    }
    gLoopInThread = this;
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
}
