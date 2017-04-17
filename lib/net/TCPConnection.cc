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
  using namespace std::placeholders;
  _dispatcher->setReadCallback(std::bind(&TCPConnection::handleRead, this, _1));
  _dispatcher->setCloseCallback(std::bind(&TCPConnection::handleClose, this));
  _dispatcher->setErrorCallback(std::bind(&TCPConnection::handleError, this));
}

TCPConnection::~TCPConnection()
{
  // TODO log here
}

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

void TCPConnection::connectionClosed()
{
  _loop->inLoopThreadOrDie();
  assert(_state == State::CONNECTED);
  setState(State::DISCONNECTED);

  // Sometimes we need to call connectionClosed
  // directly without handleClose
  _dispatcher->disable();
  _connectionCallback(shared_from_this());

  // Changes internal data of loop
  // Must be called from loop thread, else
  // not thread-safe
  _loop->removeEventDispatcher(_dispatcher.get());

  // TODO log here
}

void TCPConnection::handleRead(oplib::Timestamp receiveTime_)
{
  // Simple version, TODO: use buffer
  // We need to check for read 0 situation in handleRead
  int savedErrno = 0;
  ssize_t n = _inputBuffer.readFd(_dispatcher->fd(), &savedErrno);
  if (n > 0)
  {
    _messageCallback(shared_from_this(), &_inputBuffer, receiveTime_);
  }
  else if (n == 0)
  {
    handleClose();
  }
  else
  {
    errno = savedErrno;
    // TODO : log here
    handleError();
  }
}

void TCPConnection::handleClose()
{
  _loop->inLoopThreadOrDie();
  assert(_state == State::CONNECTED);
  _dispatcher->disable();

  // This CloseCallback binds to TCPServer/TCPClient's removeConnection
  // method
  _closeCallback(shared_from_this());
}

void TCPConnection::handleError()
{
  // TODO: log error info
  abort();
}
