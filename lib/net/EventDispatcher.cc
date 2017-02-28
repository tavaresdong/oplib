#include "EventDispatcher.h"
#include "EventLoop.h"

#include <poll.h>

namespace oplib
{
  const int EventDispatcher::kNoEvent = 0;
  const int EventDispatcher::kReadEvent = POLLIN | POLLPRI;
  const int EventDispatcher::kWriteEvent = POLLOUT;

  EventDispatcher::EventDispatcher(EventLoop* loop_, int fd_)
  : _fd(fd_), _loop(loop_), _events(0), _revents(0), _index(-1)
  {}

  void EventDispatcher::updateLoop()
  {
    _loop->updateEventDispatcher(this);
  }

  void EventDispatcher::handleEvent()
  {
    if (_revents & POLLNVAL)
    {
      // TODO log warning
    }

    if (_revents & (POLLERR | POLLNVAL))
    {
      _errorCallback();
    }

    if (_revents & (POLLIN | POLLPRI | POLLRDHUP))
    {
      _readCallback();
    }

    if (_revents & POLLOUT)
    {
      _writeCallback();
    }
  }
}
