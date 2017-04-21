#ifndef OPLIB_EVENTLOOP_H
#define OPLIB_EVENTLOOP_H

#include "Types.h"
#include "TimerManager.h"

#include <sys/types.h>
#include <atomic>
#include <memory>
#include <vector>

#include <util/Common.h>
#include <util/Timestamp.h>
#include <thread/Thread.h>
#include <thread/Mutex.h>

namespace oplib
{
  class EventDispatcher;
  class Poller;
  class TimerManager;

  class EventLoop : public Noncopyable
  {
   public:
    EventLoop();
    ~EventLoop();

    void loop();
    void inLoopThreadOrDie();
    bool inLoopThread() const
    {
      // Return true if running thread is the loop's thread
      return _threadId == CurrentThread::tid();
    }

    void quit() 
    {
      _done = true; 
      if (!inLoopThread())
      {
        // If in loop thead, the while loop inside loop()
        // will detect the _done before wait again.
        wakeup();
      }
    }

    // Update the status of EventDispatcher dp_
    void updateEventDispatcher(EventDispatcher* dp_);

    // Remove a dispatcher from EventLoop
    void removeEventDispatcher(EventDispatcher* dp_);

    // Run at a time
    TimerId runAt(const Timestamp& when_, const TimerCallback& cb_);

    // Run every time interval
    TimerId runEvery(double interval_, const TimerCallback& cb_);

    // Run after a period of time
    TimerId runAfter(double delay_, const TimerCallback& cb_);

    // Cancel a timer
    void cancel(TimerId timerId_);

    // Run a functor callback in the loop thread
    void runInLoop(const Functor& func_);
    void enqueue(const Functor& func_);

   private:

    void handleRead();
    void executePendingFunctors();
    void wakeup();

    std::atomic_bool _looping { false };
    std::atomic_bool _executingFunctors { false };
    int _timeout { 5 * 1000 };
    bool _done { false };

    const pid_t _threadId;
    std::unique_ptr<Poller> _poller;
    std::unique_ptr<TimerManager> _timerMgr;
    int _wakeupfd;
    std::unique_ptr<EventDispatcher> _wakeupDispatcher;
    std::vector<Functor> _pendingFunctors;
    oplib::Mutex _mutex;   
  };
}

#endif
