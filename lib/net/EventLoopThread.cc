#include "EventLoopThread.h"

#include <assert.h>

namespace oplib
{
  EventLoopThread::EventLoopThread()
  : _thread(std::bind(&EventLoopThread::threadFunc, this)),
    _loop(nullptr),
    _mutex(),
    _cond(_mutex),
    _running(false)
  {
  }

  EventLoopThread::~EventLoopThread()
  {
    if (_running)
    {
      _running = false;
      _loop->quit();
      _thread.join();
    }
  }

  void EventLoopThread::threadFunc()
  {
    EventLoop loop;

    {
      MutexLockGuard guard(_mutex);
      _loop = &loop;
      _cond.notify();
    }

    _running = true;
    loop.loop();
  }

  EventLoop* EventLoopThread::startLoop()
  {
    assert(!_thread.started());
    _thread.start(); // Async, in a nother thread: create loop and notify

    {
      MutexLockGuard guard(_mutex);
      while (_loop == nullptr)
      {
        _cond.wait();
      }
    }

    return _loop;
  }
}
