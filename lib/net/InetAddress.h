#ifndef OPLIB_INETADDRESS_H
#define OPLIB_INETADDRESS_H

#include <netinet/in.h>

#include <string>

struct sockaddr;

namespace oplib
{
  class InetAddress
  {
   public:
    explicit InetAddress(uint16_t port_, bool loopbackOnly_ = false, bool ipv6_ = false);
    InetAddress(const std::string& ip_, uint16_t port_, bool ipv6_ = false);
    explicit InetAddress(const struct sockaddr_in& addr_) : _addr(addr_) {}
    explicit InetAddress(const struct sockaddr_in6& addr6_) : _addr6(addr6_) {}

    void setSocketAddrInet6(const struct sockaddr_in6& addr6_) { _addr6 = addr6_; }
    sa_family_t sin_family() const
    { return _addr.sin_family; }

    // TOOD: more functions
    // std::string toIp() const;
    // std::string toPort() const;
    // std::string toIpPort() const;
    std::string toHostPort() const;

    const struct sockaddr_in& getSockAddrInet() const { return _addr; }

    const struct sockaddr* sockaddr() const;

    static bool resolve(const std::string& host_, InetAddress* result_);

   private:

    // Anonymous union
    union
    {
      struct sockaddr_in _addr;
      struct sockaddr_in6 _addr6;
    };
  };
}

#endif
