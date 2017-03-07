#ifndef OPLIB_INETADDRESS_H
#define OPLIB_INETADDRESS_H

#include <netinet/in.h>

namespace oplib
{
  class InetAddress
  {
    explicit InetAddress(uin16_t port_, bool loopbackOnly_ = false, bool ipv6_ = false);

    InetAddress(const std::string& ip_, uint16_t port_, bool ipv6_ = false);

    explicit InetAddress(const struct sockaddr_in& addr_) : _addr(addr_) {}

    explicit InetAddress(const struct sockaddr_in6& addr6_) : _addr6(addr6_) {}

    sa_family_t sin_family() const
    { return _addr.sin_family; }

    std::string toIp() const;
    std::string toPort() const;
    std::string toIpPort() const;

    static bool resolve(const std::string& host_, InetAddress* result_);

    static uint16_t hostToNetwork16(uint16_t hostaddr_);
   private:

    // Anonymous union
    union
    {
      sockaddr_in _addr;
      sockaddr_in6 _addr6;
    };
  };
}

#endif
