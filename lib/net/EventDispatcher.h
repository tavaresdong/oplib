#ifndef OPLIB_EVENTDISPATCHER_H
#define OPLIB_EVENTDISPATCHER_H

#include <functional>

#include <util/Common.h>

namespace oplib
{
  // Forward declare EventLoop
  class EventLoop;

  class EventDispatcher : Noncopyable
  {
   public:
    using EventCallback = std::function<void()>;

    EventDispatcher(EventLoop *loop_, int fd_);

    void setReadCallback(const EventCallback& cb_)
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

    int fd() const { return _fd; }
    int events() const { return _events; }
    int revents() const { return _revents; }
    void setRevents(int rev_) { _revents = rev_; }

    void enableReading() 
    {
      _events |= kReadEvent; 
      updateLoop();
    }
    // TODO enable writing...

    EventLoop* ownerLoop() { return _loop; }

    // Key method: called by loop
    void handleEvent();

    int index() const { return _index; }
    void setIndex(int ind_) { _index = ind_; } 

    bool isIgnored() const { return _events == kNoEvent; }

   private:
    void updateLoop();

    static const int kNoEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    const int _fd;
    EventLoop* _loop;

    int _events;
    int _revents;
    int _index;

    EventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
  };
}

#endif
