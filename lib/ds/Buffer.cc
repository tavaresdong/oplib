#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>

#include <util/Common.h>

using namespace oplib::ds;

const size_t Buffer::prependSize = 8;
const size_t Buffer::initialSize = 1024;

int Buffer::readFd(int fd_, int* savedErrno_)
{
  char buf[65536];
  struct iovec vec[2];
  const size_t writableLen = writableBytes();
  vec[0].iov_base = begin() + _writeIndex;
  vec[0].iov_len = writableLen;
  vec[1].iov_base = buf;
  vec[1].iov_len = sizeof(buf);

  const int iovcnt = (writableLen < sizeof(buf)) ? 2 : 1;
  const ssize_t n = ::readv(fd_, vec, iovcnt);
  if (n < 0)
  {
    *savedErrno_ = errno;
  }
  else if (implicit_cast<size_t>(n) < writableLen)
  {
    _writeIndex += n;
  }
  else
  {
    _writeIndex = _data.size();
    append(buf, n - writableLen);
  }

  return n;
}
