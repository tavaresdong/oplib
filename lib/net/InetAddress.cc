#include "InetAddress.h"
#include <strings.h>

namespace oplib
{
  
  InetAddress::InetAddress(uin16_t port_, bool loopbackOnly_ = false, bool ipv6_ = false)
  {
    if (ipv6_)
    {
      ::bzero(_addr6, sizeof(_addr6));
      _addr6.sin6_family = AF_INET6;
      in6_addr ip = loopbackOnly_ ? in6addr_loopback : in6addr_any;
      _addr6.sin6_addr = ip;

      // Convert the port to network endian
      _addr6.sin6_port = InetAddress::hostToNetwork16(port_);
    }
    else
    {
      ::bzero(_addr, sizeof(_addr));
      _addr.sin_family = AF_INET;
      in_addr_t ip = loopbackOnly_ ? INADDR_LOOPBACK : INADDR_ANY;
      _addr.sin_addr.s_addr = InetAddress::hostToNetwork32(ip);

      // Convert the port to network endian
      _addr.sin_port = InetAddress::hostToNetwork16(port_);
    } 
  }

    InetAddress(const std::string& ip_, uint16_t port_, bool ipv6_ = false);

    std::string toIp() const;
    std::string toPort() const;
    std::string toIpPort() const;

    static bool resolve(const std::string& host_, InetAddress* result_);
}
