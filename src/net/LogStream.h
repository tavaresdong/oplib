#ifndef OPLIB_NET_LOGSTREAM_H
#define OPLIB_NET_LOGSTREAM_H

#include <strings.h>

namespace oplib
{
namespace {

  const int bufsizeSmall = 4000;
  const int bufsizeLarge = 4000 * 1000;

  template <size_t SZ>
  class LogBuffer
  {
   public:
    LogBuffer(const LogBuffer&) = delete;
    LogBuffer& operator=(const LogBuffer&) = delete;

    // TODO add cookie used by gdb
    void append(const char* buf_, size_t len_);
    const char* data() const { return _buffer; }
    size_t length() const { return static_cast<int>(_cur - _buffer); }
    char* cur() { return _cur; }
    size_t remain() { return (_buffer + sizeof(_buffer)) - _cur; }
    void advance(size_t len_) { _cur += len_; }
    void reset() { _cur = _buffer; }
    void clear() { ::bzero(_buffer, sizeof(_buffer)); }

    operator std::string() { return std::string(_buffer, length()); }
    
   private:
    char _buffer[SZ];
    char* _cur { _buffer };
  };
}
}
#endif