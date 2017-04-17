#ifndef OPLIB_NET_TYPES_H
#define OPLIB_NET_TYPES_H

#include "Socket.h"
#include <ds/Buffer.h>
#include <util/Timestamp.h>

#include <functional>
#include <memory>

namespace oplib
{
  class TCPConnection;
  typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

  typedef std::function<void()> TimerCallback;
  typedef std::function<void()> EventCallback;
  typedef std::function<void(oplib::Timestamp)> ReadEventCallback;
  typedef std::function<void()> Functor;

  typedef std::function<void (std::unique_ptr<Socket>, const InetAddress&)> NewConnectionCallback;
  typedef std::function<void (const TCPConnectionPtr&)> ConnectionCallback;
  typedef std::function<void (const TCPConnectionPtr&, oplib::ds::Buffer*_, oplib::Timestamp)> MessageCallback;
  typedef std::function<void (const TCPConnectionPtr&)> CloseCallback;
}

#endif
