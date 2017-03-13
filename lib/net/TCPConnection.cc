#include "TCPConnection.h"
#include "EventDispatcher.h"

#include <unistd.h>
#include <assert.h>

using namespace oplib;

TCPConnection::TCPConnection(EventLoop *loop_,
                             const std::string& name_,
                             std::unique_ptr<Socket> sock_,
                             const InetAddress& localAddr_,
                             const InetAddress& peerAddr_)
: _state(State::CONNECTING),
  _loop(loop_),
  _name(name_),
  _sock(std::move(sock_)),
  _localAddr(localAddr_),
  _peerAddr(peerAddr_),
  _dispatcher(std::make_unique<EventDispatcher>(_loop, _sock->fd()))
{
  // TODO log
  _dispatcher->setReadCallback(std::bind(&TCPConnection::handleRead, this));
}

TCPConnection::~TCPConnection()
{}

void TCPConnection::connectionEstablished()
{
  _loop->inLoopThreadOrDie();
  assert(_state == State::CONNECTING);
  setState(State::CONNECTED);

  // Start polling for io events
  _dispatcher->enableReading();

  // Call ConnectionCallback
  if (_connectionCallback)
  {
    // Call member function shared_from_this() to create a shared_ptr
    // Don't create shared_ptr directly
    _connectionCallback(shared_from_this());
  }
}

void TCPConnection::handleRead()
{
  // Simple version, TODO: use buffer
  char buf[65536];
  ssize_t nread = ::read(_sock->fd(), buf, sizeof(buf));
  _messageCallback(shared_from_this(), buf, nread);

  // TODO: close socket if n == 0?
}
