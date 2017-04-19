#ifndef OPLIB_TCPSERVER_H
#define OPLIB_TCPSERVER_H

#include "InetAddress.h"
#include "Types.h"
#include "TCPConnection.h"
#include "Listener.h"
#include "EventLoopThreadPool.h"

#include <string>
#include <map>
#include <memory>

#include <util/Common.h>

namespace oplib
{
  class TCPServer : Noncopyable
  {
   public:

    TCPServer(EventLoop* loop_, 
              const InetAddress& address_, 
              const std::string name_,
              int nThreads_ = 0);
    ~TCPServer();

    // Thread-safe
    // Can be called multiple times
    void start();

    void setConnectionCallback(const ConnectionCallback& cb_)
    { _connectionCallback = cb_; }

    void setMessageCallback(const MessageCallback& cb_)
    { _messageCallback = cb_; }

    void setWriteCompleteCallback(const ConnectionEventCallback cb_)
    { _writeCompleteCallback = cb_; }

    // This must be called before _threadPool is started
    void setNumThreads(int nThreads_)
    { _threadPool->setNumThreads(nThreads_); }

   private:
    
    // Register to Listener, called when new connection is accepted
    // sock_ is the created socket, address_ is the peer address
    void newConnection(std::unique_ptr<Socket> sock_, const InetAddress& address_);

    void removeConnection(const TCPConnectionPtr& conn_);
    void removeConnectionInLoop(const TCPConnectionPtr& conn_);

    // Store connections
    using ConnectionMap = std::map<std::string, TCPConnectionPtr>;

    EventLoop* _loop;
    const std::string _name;
    std::unique_ptr<Listener> _listener;

    // To be passed to TCPConnections
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    ConnectionEventCallback _writeCompleteCallback;

    bool _started;
    int _nextConnId;
    ConnectionMap _connections;
    std::unique_ptr<EventLoopThreadPool> _threadPool;
  };
}

#endif
