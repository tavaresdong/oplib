#ifndef OPLIB_SOCKETUTILS_H
#define OPLIB_SOCKETUTILS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>

#include <string>
#include <cstdlib>
#include <cstdio>

namespace oplib
{
namespace socketutils
{
  // Non class member function defined in header files
  // should be declared inline (ODR rule)
  inline uint16_t hostToNetwork16(uint16_t hostaddr_)
  {
    return ::htons(hostaddr_);
  }

  inline uint32_t hostToNetwork32(uint32_t hostaddr_)
  {
    return ::htonl(hostaddr_);
  }

  inline uint16_t networkToHost16(uint16_t hostaddr_)
  {
    return ::ntohs(hostaddr_);
  }

  inline uint32_t networkToHost32(uint32_t hostaddr_)
  {
    return ::ntohl(hostaddr_);
  }

  inline void ipPortToSockAddr(const std::string& ip_, uint16_t port_, struct sockaddr_in* addr_)
  {
    addr_->sin_family = AF_INET;
    addr_->sin_port = hostToNetwork16(port_);
    int result = ::inet_pton(AF_INET, ip_.data(), &addr_->sin_addr);
    if (result == 0)
    {
      printf("Invalid address representation: %s, cannot convert to ip address\n", ip_.c_str());
      abort();
    }
    else if (result < 0)
    {
      // TODO error log
      printf("ipPortToSockAddr error\n");
      abort();
    }
  }

  inline void ipPortToSockAddr(const std::string& ip_, uint16_t port_, struct sockaddr_in6* addr6_)
  {
    addr6_->sin6_family = AF_INET6;
    addr6_->sin6_port = hostToNetwork16(port_);
    int result = ::inet_pton(AF_INET6, ip_.data(), &addr6_->sin6_addr);
    if (result == 0)
    {
      printf("Invalid address representation: %s, cannot convert to valid ipv6 address\n", ip_.c_str());
      abort();
    }
    else if (result < 0)
    {
      // TODO error log
      printf("ipPortToSockAddr error\n");
      abort();
    }
  }

  int connect(int sockfd, const struct sockaddr_in& addr);

  void shutdownWrite(int sockfd_);

  void close(int sockfd_);


  int createOrDie();

  void bindOrDie(int sockfd_, const struct sockaddr* addr_);

  void listenOrDie(int sockfd_);

  void setReuseAddrOrDie(int sockfd_, bool on_);

  int accept(int sockfd_, struct sockaddr_in6* addr_);

  const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr_);

  const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr6_);

  struct sockaddr* sockaddr_cast(struct sockaddr_in* addr_);

  struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr6_);

  std::string toHostPort(const struct sockaddr_in* addr_);

  struct sockaddr_in getLocalAddr(int sockfd_);

  int getSocketError(int sockfd_);

  bool isSelfConnect(int sockfd_);

  struct sockaddr_in getPeerAddr(int sockfd_);
}
}

#endif
