#include "TCPServer.h"
#include "SocketUtils.h"

#include <sstream>
#include <cstdio>

using namespace oplib;

TCPServer::TCPServer(EventLoop* loop_, const InetAddress& address_, const std::string name_, int nThreads_)
: _loop(loop_), _name(name_ + "_" + address_.toHostPort()),
  _listener(std::make_unique<Listener>(_loop, address_)),
  _started(false), _nextConnId(1),
  _threadPool(std::make_unique<EventLoopThreadPool>(_loop))
{
  // Listener will call TCPServer::newConnection to establish 
  // the TCPConnection
  _listener->setNewConnectionCallback(std::bind(&TCPServer::newConnection, this,
    std::placeholders::_1, std::placeholders::_2));
  _threadPool->setNumThreads(nThreads_);
}

TCPServer::~TCPServer()
{}

void TCPServer::start()
{
  if (!_started)
  {
    _started = true;
    _threadPool->start();
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
  oss << _name << "_" << _nextConnId;

  // Connection name is TCPServer name(ip + port) + conn ID
  const std::string connName = oss.str();
  // TODO log
  
  // localaddr is the newly created address at local host for the incoming connection
  InetAddress localAddr(socketutils::getLocalAddr(sock_->fd()));

  EventLoop* dispatchedLoop = _threadPool->getNextLoop();

  // std::make_shared is used to save one memory allocation
  printf("Choosing loop: %p\n", static_cast<void*>(dispatchedLoop));
  TCPConnectionPtr conn(std::make_shared<TCPConnection>(dispatchedLoop, connName, std::move(sock_),
                                                        localAddr, peerAddress_));
  printf("Connection name is %s\n", connName.c_str());
  _connections[connName] = conn;
  auto cnt = _connections.count(connName);
  assert(cnt == 1);
  conn->setConnectionCallback(_connectionCallback);
  conn->setMessageCallback(_messageCallback);
  conn->setCloseCallback(std::bind(&TCPServer::removeConnection, this, std::placeholders::_1));
  conn->setWriteCompleteCallback(_writeCompleteCallback);

  // This will call the _connectionCallback
  dispatchedLoop->runInLoop(std::bind(&TCPConnection::connectionEstablished, conn));
} 

void TCPServer::removeConnection(const TCPConnectionPtr& conn_)
{
  _loop->runInLoop(std::bind(&TCPServer::removeConnectionInLoop, this, conn_));
}

void TCPServer::removeConnectionInLoop(const TCPConnectionPtr& conn_)
{
  _loop->inLoopThreadOrDie();
  printf("Erasing conn %s from _connections\n", conn_->name().c_str());
  auto n = _connections.erase(conn_->name());
  assert(n == 1);
  UNUSED(n);

  EventLoop* dispatchedLoop = conn_->getLoop();
  // Extend the lifetime of conn_ to connectionClosed is called
  dispatchedLoop->enqueue(std::bind(&TCPConnection::connectionClosed, conn_));
}

