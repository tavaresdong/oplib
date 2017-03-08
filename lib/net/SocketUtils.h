#ifndef OPLIB_SOCKETUTILS_H
#define OPLIB_SOCKETUTILS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>

namespace oplib
{
namespace socketutils
{
  uint16_t hostToNetwork16(uint16_t hostaddr_)
  {
    return ::htons(hostaddr_);
  }

  uint32_t hostToNetwork32(uint32_t hostaddr_)
  {
    return ::htonl(hostaddr_);
  }

  void ipPortToSockaddr(const std::string& ip_, uin16_t port_, struct sockaddr_in* addr_)
  {
    addr_.sin_family = AF_INET;
    addr_.sin_port = hostToNetwork16(port_);
    if (::inet_pton(AF_INET, ip_.data(), &addr_->sin_addr))
    {
      // TODO error log
      assert(false);
    }
  }

  void ipPortToSockaddr(const std::string& ip_, uin16_t port_, struct sockaddr_in6* addr_)
  {
    addr_.sin_family = AF_INET6;
    addr_.sin_port = hostToNetwork16(port_);
    if (::inet_pton(AF_INET6, ip_.data(), &addr_->sin6_addr))
    {
      // TODO error log
      assert(false);
    }
  }

  int createOrDie();

  void bindOrDie(int sockfd_, const struct sockaddr* addr_);

  void listenOrDie(int sockfd_);

  void setReuseAddrOrDie(int sockfd_, bool on_);

  int accept(int sockfd_, struct socketaddr_in* addr_);

  const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr_);

  const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr6_);


}
}

#endif
