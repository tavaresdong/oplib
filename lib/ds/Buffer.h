#ifndef OPLIB_DS_BUFFER_H
#define OPLIB_DS_BUFFER_H

#include <vector>

namespace oplib
{
namespace ds
{
  class Buffer
  {
    constexpr const size_t prependSize { 8 };
    constexpr const size_t initialSize { 1024 };

   public:
    Buffer(size_t prependable_ = prependSize );

    int readableBytes() const
    {
      return _writeIndex - _readIndex;
    }

    int writebleBytes() const
    {
      return _data.size() - writeIndex;
    }

    const char* peek();

    void retrieve(int sz_);

    std::string retrieveAsString(int sz_);

    // TODO
    void append(const char* data_, size_t len);

    // TODO
    void prepend(const char* data_, size_t len);

    // TODO 
    void swap(Buffer& buf);

    // TODO
    int readFd(int fd_);

   private:

    void reset();

    const size_t prependable
    int _readIndex;
    int _writeIndex;
    std::vector<char> _data;
  };
}
}

#endif
