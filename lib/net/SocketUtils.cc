#include "SocketUtils.h"
#include <util/Common.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>


namespace oplib
{
namespace socketutils
{

int createOrDie()
{
  int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    IPPROTO_TCP);
  if (fd < 0)
  {
    // TODO error log
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
    abort();
  }
}

void setReuseAddrOrDie(int sockfd_, bool on_)
{
  int optval = on_ ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval);
  if (ret < 0)
  {
    // TODO error log
    abort();
  }
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr_)
{
  return static_cast<const struct sockaddr*>(implicit_cast<void*>(addr_));
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr6_)
{
  return static_cast<const struct sockaddr*>(implicit_cast<void*>(addr6_));
}

int accept(int sockfd_, struct socketaddr_in* addr_)
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
        abort();
    }
  }

  return connfd;
}

}
}
