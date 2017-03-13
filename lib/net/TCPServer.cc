#include "TCPServer.h"
#include "SocketUtils.h"

#include <sstream>

using namespace oplib;

TCPServer::TCPServer(EventLoop* loop_, const InetAddress& address_, const std::string name_)
: _loop(loop_), _name(name_ + " " + address_.toHostPort()),
  _listener(std::make_unique<Listener>(_loop, address_)),
  _started(false), _nextConnId(1)
{
  // Listener will call TCPServer::newConnection to establish 
  // the TCPConnection
  _listener->setNewConnectionCallback(std::bind(&TCPServer::newConnection, this,
    std::placeholders::_1, std::placeholders::_2));
}

TCPServer::~TCPServer()
{}

void TCPServer::start()
{
  if (!_started)
  {
    _started = true;
  }
  if (!_listener->listening())
  {
    // Run in loop, so start() can be called from another thread
    _loop->runInLoop(std::bind(&Listener::listen, _listener.get()));
  }
}

void TCPServer::newConnection(std::unique_ptr<Socket> sock_, const InetAddress& peerAddress_)
{
  // Create the TCPConnection
  // This should be called in the loop thread
  _loop->inLoopThreadOrDie();

  std::ostringstream oss;
  oss << _name << _nextConnId;

  // Connection name is TCPServer name(ip + port) + conn ID
  const std::string connName = oss.str();
  // TODO log
  
  // localaddr is the newly created address at local host for the incoming connection
  InetAddress localAddr(socketutils::getLocalAddr(sock_->fd()));

  // std::make_shared is used to save one memory allocation
  TCPConnectionPtr conn(std::make_shared<TCPConnection>(_loop, connName, std::move(sock_),
                                                        localAddr, peerAddress_));
  _connections[connName] = conn;
  conn->setConnectionCallback(_connectionCallback);
  conn->setMessageCallback(_messageCallback);

  // This will call the _connectionCallback
  conn->connectionEstablished();
} 
