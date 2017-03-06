#ifndef OPLIB_EVENTLOOP_H
#define OPLIB_EVENTLOOP_H

#include <util/Common.h>
#include <util/Timestamp.h>
#include <thread/Thread.h>
#include <thread/Mutex.h>

#include <sys/types.h>
#include <atomic>
#include <memory>
#include <vector>

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

    // TODO
    void updateEventDispatcher(EventDispatcher* dp_);

    // Run at a time
    void runAt(const Timestamp& when_, const TimerCallback& cb_);

    // Run every time interval
    void runEvery(double interval_, const TimerCallback& cb_);

    // Run after a period of time
    void runAfter(double delay_, const TimerCallback& cb_);

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
