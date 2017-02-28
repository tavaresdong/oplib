#ifndef OPLIB_EVENTLOOP_H
#define OPLIB_EVENTLOOP_H

#include <util/Common.h>
#include <thread/Thread.h>

#include <sys/types.h>
#include <atomic>
#include <memory>

namespace oplib
{
  class EventDispatcher;
  class Poller;

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

    void quit() { _done = true; }

    // TODO
    void updateEventDispatcher(EventDispatcher* dp_);

   private:
    std::atomic_bool _looping { false };
    int _timeout { 5 * 1000 };
    bool _done { false };
    const pid_t _threadId;
    std::unique_ptr<Poller> _poller;
  };
}

#endif