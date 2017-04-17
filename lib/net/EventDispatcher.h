#ifndef OPLIB_EVENTDISPATCHER_H
#define OPLIB_EVENTDISPATCHER_H

#include "Types.h"
#include <util/Common.h>

#include <functional>

namespace oplib
{
  class EventLoop;

  class EventDispatcher : Noncopyable
  {
   public:

    EventDispatcher(EventLoop *loop_, int fd_);
    ~EventDispatcher();

    void setReadCallback(const ReadEventCallback& cb_)
    {
      _readCallback = cb_;
    }

    void setWriteCallback(const EventCallback& cb_)
    {
      _writeCallback = cb_;
    }

    void setErrorCallback(const EventCallback& cb_)
    {
      _errorCallback = cb_;
    }

    void setCloseCallback(const EventCallback& cb_)
    {
      _closeCallback = cb_;
    }

    int fd() const { return _fd; }
    int events() const { return _events; }
    int revents() const { return _revents; }
    void setRevents(int rev_) { _revents = rev_; }

    void enableReading() 
    {
      _events |= kReadEvent; 
      updateLoop();
    }

    void enableWriting()
    {
      _events |= kWriteEvent;
      updateLoop();
    }

    void disableWriting()
    {
      _events &= ~kWriteEvent;
      updateLoop();
    }

    bool isWriting()
    { return _events & kWriteEvent; }

    void disable()
    {
      _events = kNoEvent;
      updateLoop();
    }


    EventLoop* ownerLoop() { return _loop; }

    void handleEvent(oplib::Timestamp receiveTime_);

    int index() const { return _index; }
    void setIndex(int ind_) { _index = ind_; } 

    bool isIgnored() const { return _events == kNoEvent; }

   private:
    void updateLoop();

    static const int kNoEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    bool _handlingEvent;

    // EventDispatcher does not own the fd
    const int _fd;
    EventLoop* _loop;

    int _events;
    int _revents;
    int _index;

    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
    EventCallback _closeCallback;
  };
}

#endif
