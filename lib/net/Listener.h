#ifndef OPLIB_LISTENER_H
#define OPLIB_LISTENER_H

#include <util/Common.h>
#include "Socket.h"
#include "EventDispatcher.h"

namespace oplib
{
  class EventLoop;

  class Listener : Noncopyable
  {
   public:
    using NewConnectionCallback = std::function<void (std::unique_ptr<Socket>, const InetAddress&)>;

    Listener(EventLoop* loop_, const InetAddress& listenAddr_);

    void setNewConnectionCallback(const NewConnectionCallback& cb_)
    { _newConnectionCb = cb_; }

    bool listening() const
    { return _listening; }

    void listen();

   private:

    void handleRead();

    EventLoop* _loop;
    bool _listening;
    NewConnectionCallback _newConnectionCb;
    Socket _listenSock;
    EventDispatcher _dispatcher;
  }
}

#endif
