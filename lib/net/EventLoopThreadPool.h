#ifndef OPLIB_EVENTLOOPTHREADPOOL_H
#define OPLIB_EVENTLOOPTHREADPOOL_H

#include "EventLoop.h"
#include "EventLoopThread.h"

#include <memory>

#include <util/Common.h>

namespace oplib
{
  class EventLoopThreadPool : Noncopyable
  {
   public:
    EventLoopThreadPool(EventLoop* masterLoop_);
    ~EventLoopThreadPool();

    void setNumThreads(size_t nThreads_) 
    {
      if (_started) return; // TODO log
      _nThreads = nThreads_; 
    }

    void start();
    EventLoop* getNextLoop();

   private:
    EventLoop* _masterLoop;
    bool _started;
    size_t _nThreads;
    int _next;
    std::vector<std::shared_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;
  };
}

#endif
