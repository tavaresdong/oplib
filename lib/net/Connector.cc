#include "Connector.h"
#include "SocketUtils.h"

#include <errno.h>
#include <string.h>

using namespace oplib::net;

const int Connector::initRetryMs = 500;
const int Connector::maxRetryMs = 30 * 1000;

Connector::Connector(EventLoop* loop_, const InetAddress& serverAddr_)
: _loop(loop_),
  _serverAddr(serverAddr_),
  _connect(false),
  _state(State::DISCONNECTED),
  _retryDelay(initRetryMs)
{}

Connector::~Connector()
{
  // TODO: cancel the timer
  //_loop->cancel(timerid);
  assert(!_dispatcher);
}

void Connector::start()
{
  _connect = true;
  _loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
  _loop->inLoopThreadOrDie();
  assert(_state == State::DISCONNECTED);
  printf("Connector::startInLoop()\n");
  if (_connect)
  {
    connect();
  }
  else
  {
    //TODO log
    printf("Connector::startInLoop(): do not connect\n");
  }
}

void Connector::connect()
{
  int sockfd = socketutils::createOrDie();
  // TODO sockets::connect
  int ret = socketutils::connect(sockfd, _serverAddr.getSockAddrInet());
  int savedErrno = (ret == 0) ? 0 : errno;

  switch (savedErrno)
  {
   case 0:
   case EINPROGRESS:
   case EINTR:
   case EISCONN:
     printf("Connector::connect(): connecting\n");
     connecting(sockfd);
     break;
   case EACCES:
   case EPERM:
   case EAFNOSUPPORT:
   case EALREADY:
   case EBADF:
   case EFAULT:
   case ENOTSOCK:
     // TODO: log
     printf("connect error in Connector::startInLoop\n");
     socketutils::close(sockfd);
     break;
   default:
     printf("unexpected error in Connector::startInLoop with errno: %d\n", savedErrno);
     socketutils::close(sockfd);
     break;
  }
}

void Connector::restart()
{
  _loop->inLoopThreadOrDie();
  setState(State::DISCONNECTED);
  _retryDelay = initRetryMs;
  _connect = true;
  startInLoop();
}

void Connector::stop()
{
  _connect = false;
  //_loop->cancel(timerID);
}

void Connector::connecting(int sockfd_)
{
  printf("Connector()::connecting()\n");
  setState(State::CONNECTING);
  assert(!_dispatcher);
  _dispatcher.reset(new EventDispatcher(_loop, sockfd_));
  _dispatcher->setWriteCallback(std::bind(&Connector::handleWrite, this));
  _dispatcher->setErrorCallback(std::bind(&Connector::handleError, this));
  _dispatcher->enableWriting();
}

int Connector::removeAndResetDispatcher()
{
  _dispatcher->disable();
  _loop->removeEventDispatcher(_dispatcher.get());
  int sockfd = _dispatcher->fd();

  // This method is called inside handleEvent
  _loop->enqueue(std::bind(&Connector::resetDispatcher, this));
  return sockfd;
}

void Connector::resetDispatcher()
{
  _dispatcher.reset();
}

void Connector::handleWrite()
{
  printf("Connector::handleWrite\n");

  if (_state == State::CONNECTING)
  {
    int sockfd = removeAndResetDispatcher();
    int err = socketutils::getSocketError(sockfd);
    if (err)
    {
      printf("Connector::handleWrite - SO_ERROR = %d\n", err);
      retry(sockfd);
    }
    else if (socketutils::isSelfConnect(sockfd))
    {
      printf("Connector::handleWrite - Self connect\n");
      retry(sockfd);
    }
    else
    {
      setState(State::CONNECTED);
      if (_connect)
      {
        _newConnectionCallback(sockfd);
      }
      else
      {
        socketutils::close(sockfd);
      }
    }
  }
  else
  {
    assert(_state == State::DISCONNECTED);
  }
}

void Connector::handleError()
{
  printf("Connector::handleError\n");
  assert(_state == State::CONNECTING);

  int sockfd = removeAndResetDispatcher();
  int err = socketutils::getSocketError(sockfd);
  printf("SO_ERROR = %d %s\n", err, ::strerror(err));
  retry(sockfd);
}

void Connector::retry(int sockfd)
{
  socketutils::close(sockfd);
  setState(State::DISCONNECTED);
  if (_connect)
  {
    // TODO use a timerid to record the timer so we can cancel if needed
    _loop->runAfter(_retryDelay / 1000.0, std::bind(&Connector::startInLoop, this));
    _retryDelay = std::min(maxRetryMs, 2 * _retryDelay);
  }
  else
  {
    printf("Do not connect\n");
  }
}
