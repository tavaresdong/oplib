#include "Socket.h"
#include "InetAddress.h"
#include "SocketUtils.h"

#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace oplib;

Socket::~Socket()
{
  socketutils::close(_sockfd);
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

void Socket::shutdownWrite()
{ socketutils::shutdownWrite(_sockfd); }

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

