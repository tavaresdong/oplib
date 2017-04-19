#include "EventDispatcher.h"
#include "EventLoop.h"

#include <poll.h>
#include <cassert>

namespace oplib
{
  const int EventDispatcher::kNoEvent = 0;
  const int EventDispatcher::kReadEvent = POLLIN | POLLPRI;
  const int EventDispatcher::kWriteEvent = POLLOUT;

  EventDispatcher::EventDispatcher(EventLoop* loop_, int fd_)
  : _handlingEvent(false),
    _fd(fd_), _loop(loop_),
    _events(0),
    _revents(0),
    _index(-1)
  {}

  EventDispatcher::~EventDispatcher()
  {
    assert(!_handlingEvent);
  }

  void EventDispatcher::updateLoop()
  {
    _loop->inLoopThreadOrDie();
    _loop->updateEventDispatcher(this);
  }

  void EventDispatcher::handleEvent(oplib::Timestamp receiveTime_)
  {
    _handlingEvent = true;

    if (_revents & POLLNVAL)
    {
      // TODO log warning
    }

    if ((_revents & POLLHUP) && !(_revents & POLLIN))
    {
      // Peer closed, bust stil data to read, read
      // will return 0 only after all data in the channel are read
      if (_closeCallback)
        _closeCallback();
    }

    if (_revents & (POLLERR | POLLNVAL))
    {
      if (_errorCallback)
        _errorCallback();
    }

    if (_revents & (POLLIN | POLLPRI | POLLRDHUP))
    {
      if (_readCallback)
        _readCallback(receiveTime_);
    }

    if (_revents & POLLOUT)
    {
      if (_writeCallback)
        _writeCallback();
    }

    _handlingEvent = false;
  }
}
