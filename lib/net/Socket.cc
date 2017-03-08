#include "Socket.h"
#include "InetAddress.h"
#include "SocketUtils.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace oplib;

Socket::~Socket()
{
  int ret = ::close(_sockfd);
  // TODO error log
  assert(ret == 0);
}

void Socket::bindAddress(const InetAddress& listenAddr_)
{
  socketutils::bindOrDie(_sockfd, listenAddr_.sockaddr());
}

void Socket::listen()
{
  socketutils::listenOrDie(_sockfd);
}

void Socket::setReuseAddr(bool on_)
{
  socketutils::setReuseAddrOrDie(_sockfd, on_);
}

int Socket::accept(InetAddress* peer_)
{
  struct sockaddr_in6 addr;
  ::bzero(&addr, sizeof(addr));
  int connfd = socketutils::accept(_sockfd, &addr);

  if (connfd > 0)
  {
    peer_->setSocketAddrInet6(addr);
  }

  return connfd;
}

