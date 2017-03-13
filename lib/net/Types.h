#ifndef OPLIB_NET_TYPES_H
#define OPLIB_NET_TYPES_H

#include <functional>
#include <memory>

#include "Socket.h"

namespace oplib
{
  class TCPConnection;
  typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

  typedef std::function<void()> TimerCallback;
  typedef std::function<void()> EventCallback;
  typedef std::function<void()> Functor;

  typedef std::function<void (std::unique_ptr<Socket>, const InetAddress&)> NewConnectionCallback;
  typedef std::function<void (const TCPConnectionPtr&)> ConnectionCallback;
  typedef std::function<void (const TCPConnectionPtr&, const char *data_, ssize_t)> MessageCallback;
}

#endif
