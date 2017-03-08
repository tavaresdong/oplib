#include "InetAddress.h"
#include "SockeUtils.h"
#include <strings.h>
#include <assert.h>
#include <netdb.h>
#include <sys/socket.h>

InetAddress::InetAddress(uin16_t port_, bool loopbackOnly_ = false, bool ipv6_ = false)
{
  if (ipv6_)
  {
    ::bzero(_addr6, sizeof(_addr6));
    _addr6.sin6_family = AF_INET6;
    in6_addr ip = loopbackOnly_ ? in6addr_loopback : in6addr_any;
    _addr6.sin6_addr = ip;

    // Convert the port to network endian
    _addr6.sin6_port = socketutils::hostToNetwork16(port_);
  }
  else
  {
    ::bzero(_addr, sizeof(_addr));
    _addr.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly_ ? INADDR_LOOPBACK : INADDR_ANY;
    _addr.sin_addr.s_addr = socketutils::hostToNetwork32(ip);

    // Convert the port to network endian
    _addr.sin_port = socketutils::hostToNetwork16(port_);
  } 
}

InetAddress::InetAddress(const std::string& ip_, uint16_t port_, bool ipv6_ = false)
{
  if (ipv6_)
  {
    ::bzero(&_addr6, sizeof(_addr6));
    socketutils::ipPortToSockAddr(ip_, port_, &_addr6);
  }
  else
  {
    ::bzero(&_addr, sizeof(_addr));
    socketutils::ipPortToSockAddr(ip_, port_, &_addr);
  }
}

const struct sockaddr* sockaddr() const
{
  return static_cast<const struct sockaddr*>(implicit_cast<void*>(_addr6));
}

static __thread char gResolveBuf[64 * 1024];

bool InetAddress::resolve(const std::string& host_, InetAddress* result_)
{
  assert(result_ != nullptr);   
  struct hostent hent;
  struct hostent *he = nullptr;
  int herrno = 0;
  ::bzero(hent, sizeof(hent));

  int ret = ::gethostbyname_r(host_.c_str(), &hent, gResolveBuf, sizeof(gResolveBuf), &he, &herrno);
  if (ret == 0 && he != nullptr)
  {
    out_->sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return true;
  }
  else
  {
    if (ret)
    {
      // TODO error log
      assert(false);
    }
    return false;
  }
}
