#ifndef OPLIB_TCPSERVER_H
#define OPLIB_TCPSERVER_H

#include <string>
#include <map>
#include <memory>

#include <util/Common.h>
#include "InetAddress.h"
#include "Types.h"
#include "TCPConnection.h"
#include "Listener.h"

namespace oplib
{
  class TCPServer : Noncopyable
  {
   public:

    TCPServer(EventLoop* loop_, const InetAddress& address_, const std::string name_);
    ~TCPServer();

    // Thread-safe
    // Can be called multiple times
    void start();

    void setConnectionCallback(const ConnectionCallback& cb_)
    { _connectionCallback = cb_; }

    void setMessageCallback(const MessageCallback& cb_)
    { _messageCallback = cb_; }

   private:
    
    // Register to Listener, called when new connection is accepted
    // sock_ is the created socket, address_ is the peer address
    void newConnection(std::unique_ptr<Socket> sock_, const InetAddress& address_);

    // Store connections
    using ConnectionMap = std::map<std::string, TCPConnectionPtr>;

    EventLoop* _loop;
    const std::string _name;
    std::unique_ptr<Listener> _listener;

    // To be passed to TCPConnections
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    bool _started;
    int _nextConnId;
    ConnectionMap _connections;
  };
}

#endif
