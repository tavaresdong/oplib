#include "EventLoopThreadPool.h"

#include <cassert>

using namespace oplib;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* masterLoop_)
: _masterLoop(masterLoop_),
  _started(false),
  _nThreads(0),
  _next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start()
{
  assert(!_started);
  _masterLoop->inLoopThreadOrDie();

  _started = true;

  for (size_t i = 0; i < _nThreads; ++i)
  {
    auto loopThread = std::make_shared<EventLoopThread>();
    _threads.push_back(loopThread);
    _loops.push_back(loopThread->startLoop());
  }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  _masterLoop->inLoopThreadOrDie();

  // If single-threaded, just use the master loop
  auto loop = _masterLoop;

  if (!_loops.empty())
  {
    loop = _loops[_next];
    ++_next;
    if (static_cast<size_t>(_next) >= _loops.size())
    {
      _next = 0;
    }
  }

  return loop;
}
