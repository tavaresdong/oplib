#include "TCPConnection.h"
#include "EventDispatcher.h"
#include "SocketUtils.h"
#include <util/Common.h>

#include <unistd.h>
#include <signal.h>
#include <cassert>
#include <cstdio>
#include <string.h>

using namespace oplib;

namespace detail
{
  // Use a global object to ignore sigpipe
  class IgnoreSigPipe
  {
   public:
    IgnoreSigPipe()
    {
      ::signal(SIGPIPE, SIG_IGN);
    }
  };

  IgnoreSigPipe gIgnore;
}

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
  _dispatcher->setWriteCallback(std::bind(&TCPConnection::handleWrite, this));
  _dispatcher->setCloseCallback(std::bind(&TCPConnection::handleClose, this));
  _dispatcher->setErrorCallback(std::bind(&TCPConnection::handleError, this));
}

TCPConnection::~TCPConnection()
{
  // TODO log here
  printf("TCPConnection::~TCPConnection() called\n");
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
  assert(_state == State::CONNECTED ||
         _state == State::DISCONNECTING);
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

void TCPConnection::handleWrite()
{
  _loop->inLoopThreadOrDie();
  if (_dispatcher->isWriting())
  {
    ssize_t nwrite = ::write(_dispatcher->fd(),
                             _outputBuffer.peek(),
                             _outputBuffer.readableBytes());
    if (nwrite > 0)
    {
      _outputBuffer.retrieve(nwrite);
      if (_outputBuffer.readableBytes() == 0u)
      {
        _dispatcher->disableWriting();
        if (_writeCompleteCallback)
        {
          _loop->enqueue(std::bind(_writeCompleteCallback, shared_from_this()));
        }
        if (_state == State::DISCONNECTING)
        {
          shutdownInLoop();
        }
      }
      else
      {
        // TODO log: ready to write more data
      }
    }
    else
    {
      // TODO log write error
      printf("Error writing data to peer\n");
    }
  }
  else
  {
    // TODO: log connection is down, no more writing
    printf("Connection is down, no more writing\n");
  }
}

void TCPConnection::handleClose()
{
  _loop->inLoopThreadOrDie();
  assert(_state == State::CONNECTED ||
         _state == State::DISCONNECTING);
  _dispatcher->disable();

  // This CloseCallback binds to TCPServer/TCPClient's removeConnection
  _closeCallback(shared_from_this());
}

void TCPConnection::handleError()
{
  // TODO: log error info
  int err = socketutils::getSocketError(_dispatcher->fd());
  printf("TCPConnection::handleError() error is %s\n", ::strerror(err));
}

void TCPConnection::send(const std::string& message_)
{
  if (_state == State::CONNECTED)
  {
    if (_loop->inLoopThread())
    {
      sendInLoop(message_);
    }
    else
    {
      // Make a copy of message_ and send in loop thread
      _loop->runInLoop(std::bind(&TCPConnection::sendInLoop, this, message_));
    }
  } 
}

void TCPConnection::sendInLoop(const std::string& message_)
{
  _loop->inLoopThreadOrDie();
  ssize_t nwrite = 0;
  if (!_dispatcher->isWriting() && _outputBuffer.readableBytes() == 0u)
  {
    // Try to write directly if writing and no pending data
    nwrite = ::write(_dispatcher->fd(), message_.data(), message_.size());
    if (nwrite >= 0)
    {
      if (implicit_cast<size_t>(nwrite) < message_.size())
      {
        // TODO log
      }
      else if (_writeCompleteCallback)
      {
        // Write is complete, trigger _writeCompleteCallback
        _loop->runInLoop(std::bind(_writeCompleteCallback, shared_from_this()));
      }
    }
    else
    {
      // TODO: log
      nwrite = 0;
      if (errno == EPIPE)
      {
        // Peer is down
        handleClose(); 
      }
      else if (errno != EWOULDBLOCK)
      {
        // TODO log
        printf("Error sending data, not receiving EWOULDBLOCK\n");
        abort();
      }
    }

    assert(nwrite >= 0);
    if (implicit_cast<size_t>(nwrite) < message_.size())
    {
      _outputBuffer.append(message_.data() + nwrite, message_.size() - nwrite);
      if (!_dispatcher->isWriting())
      {
        // Remaining data to write, inform the dispatcher
        // to watch writing event
        _dispatcher->enableWriting();
      }
    }
  }
}

void TCPConnection::shutdown()
{
  if (_state == State::CONNECTED)
  {
    setState(State::DISCONNECTING);
    _loop->runInLoop(std::bind(&TCPConnection::shutdownInLoop, this));
  }
}

void TCPConnection::shutdownInLoop()
{
  _loop->inLoopThreadOrDie();
  if (!_dispatcher->isWriting())
  {
    _sock->shutdownWrite();
  }
}

void TCPConnection::enableTcpNoDelay()
{
  _sock->setTcpNoDelay(true);
}

void TCPConnection::disableTcpNoDelay()
{
  _sock->setTcpNoDelay(false);
}
