#ifndef OPLIB_SOCKET_H
#define OPLIB_SOCKET_H

#include <util/Common.h>

namespace oplib
{
  class InetAddress;

  class Socket : Noncopyable
  {
   public:
    explicit Socket(int fd_) : _sockfd(fd_) {}

    ~Socket();

    void bindAddress(const InetAddress& listenAddr_);

    void listen();

    int fd() const { return _sockfd; }

    void setReuseAddr(bool on_);

    int accept(InetAddress* peer_);

   private:
    const int _sockfd;
  };
}

#endif
