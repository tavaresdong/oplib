#ifndef OPLIB_EVENTLOOP_THREAD_H
#define OPLIB_EVENTLOOP_THREAD_H

#include <thread/Thread.h>
#include <thread/Mutex.h>
#include <thread/Condition.h>
#include "EventLoop.h"

namespace oplib
{
  class EventLoopThread
  {
   public:

    EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();

   private:
    void threadFunc();

    Thread _thread;
    EventLoop* _loop;
    Mutex _mutex;
    Condition _cond;
    bool _running;
  };
}

#endif
