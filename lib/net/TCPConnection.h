#ifndef OPLIB_TCPCONNECTION_H
#define OPLIB_TCPCONNECTION_H

#include <ds/Buffer.h>
#include <util/Common.h>
#include <util/Timestamp.h>

#include "EventLoop.h"
#include "InetAddress.h"

namespace oplib
{
  class EventDispatcher;

  class TCPConnection : Noncopyable,
                        public std::enable_shared_from_this<TCPConnection>
  {
   public:
    TCPConnection(EventLoop* loop_,
                  const std::string& name_,
                  std::unique_ptr<Socket> sock_,
                  const InetAddress& localAddr_,
                  const InetAddress& peerAddr_);

    // This must be defined in implementation
    // because we have std::unique_ptr
    ~TCPConnection();

    bool connected() const
    { return _state == State::CONNECTED; }

    std::string name() const
    { return _name; }

    InetAddress localAddr() const
    { return _localAddr; }

    InetAddress peerAddr() const
    { return _peerAddr; }

    void setConnectionCallback(const ConnectionCallback& cb_)
    { _connectionCallback = cb_; }

    void setMessageCallback(const MessageCallback& cb_)
    { _messageCallback = cb_; }   

    void setCloseCallback(const CloseCallback& cb_)
    { _closeCallback = cb_; }

    void setWriteCompleteCallback(const ConnectionEventCallback& cb_)
    { _writeCompleteCallback = cb_; }

    // Called by the TCPServer in loop thread
    void connectionEstablished();
    void connectionClosed(); 

    // send() is thread-safe, can be called from another thread TODO
    void send(const std::string& message_);

    // shutdown() is thread-safe TODO
    void shutdown();

    void enableTcpNoDelay();
    void disableTcpNoDelay();

    EventLoop* getLoop() { return _loop; }
   private:

    // Register to EventDispatcher
    // for handling reading event
    void handleRead(oplib::Timestamp receiveTime_);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string& message_);
    void shutdownInLoop();

    enum class State 
    { 
      CONNECTING,
      CONNECTED,
      DISCONNECTING,
      DISCONNECTED
    };

    void setState(State s_) { _state = s_; }
    
    State _state;

    EventLoop* _loop;
    const std::string _name;

    // TCPConnection owns _sock!
    // Need to close the socket when connection is closed
    std::unique_ptr<Socket> _sock;

    InetAddress _localAddr;
    InetAddress _peerAddr;
    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    ConnectionEventCallback _writeCompleteCallback;

    std::unique_ptr<EventDispatcher> _dispatcher;

    oplib::ds::Buffer _inputBuffer;
    oplib::ds::Buffer _outputBuffer;
  };

  typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;
}

#endif
