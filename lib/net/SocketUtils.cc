#include "SocketUtils.h"
#include <util/Common.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

namespace oplib
{
namespace socketutils
{

void close(int sockfd_)
{
  int ret = ::close(sockfd_);
  if (ret < 0)
  {
    // TODO: error log
    printf("Close socket error\n");
    abort();
  }
}

int createOrDie()
{
  int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    IPPROTO_TCP);
  if (fd < 0)
  {
    // TODO error log
    printf("Create socket error\n");
    abort();
  }
  return fd;
}

void bindOrDie(int sockfd_, const struct sockaddr* addr_)
{
  int ret = ::bind(sockfd_, addr_, static_cast<socklen_t>(sizeof(struct sockaddr_in6))); 
  if (ret < 0)
  {
    // TODO error log
    printf("Bind socket error\n");
    abort();
  }
}

void listenOrDie(int sockfd_)
{
  // backlog: waiting queue length
  int ret = ::listen(sockfd_, SOMAXCONN);
  if (ret < 0)
  {
    // TODO: error log
    printf("Listen socket error\n");
    abort();
  }
}

void setReuseAddrOrDie(int sockfd_, bool on_)
{
  int optval = on_ ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  if (ret < 0)
  {
    // TODO error log
    printf("Set reuse socket error\n");
    abort();
  }
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr_)
{
  return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr_));
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr6_)
{
  return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr6_));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr_)
{
  return static_cast<struct sockaddr*>(implicit_cast<void*>(addr_));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr6_)
{
  return static_cast<struct sockaddr*>(implicit_cast<void*>(addr6_));
}

int accept(int sockfd_, struct sockaddr_in6* addr_)
{
  socklen_t len = sizeof(*addr_);
  int connfd = ::accept4(sockfd_, sockaddr_cast(addr_),
                         &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0)
  {
    int savederrno = errno;
    switch (savederrno)
    {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPERM:
      case EMFILE:
        errno = savederrno;
        break;
      default:
        // TODO error log
        // TODO check other error codes
        printf("Accept socket error\n");
        abort();
    }
  }

  return connfd;
}

std::string toHostPort(const struct sockaddr_in* addr_)
{
  char host[INET_ADDRSTRLEN] = "INVALID";
  ::inet_ntop(AF_INET, &addr_->sin_addr, host, sizeof(host));
  int port = networkToHost16(addr_->sin_port);

  char buf[32];
  snprintf(buf, sizeof(buf), "%s:%d", host, port);
  return buf;
}

struct sockaddr_in getLocalAddr(int sockfd_)
{
  struct sockaddr_in localaddr;
  ::bzero(&localaddr, sizeof(localaddr));
  socklen_t addrlen = sizeof(localaddr);
  if (::getsockname(sockfd_, sockaddr_cast(&localaddr), &addrlen) < 0)
  {
    // TODO error log
    printf("get local addr error\n");
    abort();
  }

  return localaddr;
}

void shutdownWrite(int sockfd_)
{
  if (::shutdown(sockfd_, SHUT_WR) < 0)
  {
    // TODO : log error
    printf("shutdown write error\n");
    abort();
  }
}

}
}
