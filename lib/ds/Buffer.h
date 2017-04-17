#ifndef OPLIB_DS_BUFFER_H
#define OPLIB_DS_BUFFER_H

#include <vector>
#include <cassert>
#include <algorithm>
#include <utility>

namespace oplib
{
namespace ds
{
  class Buffer
  {
   public:

    static const size_t prependSize;
    static const size_t initialSize;

    Buffer(size_t prependable_ = prependSize )
    : _prependable(prependable_),
      _readIndex(_prependable),
      _writeIndex(_prependable),
      _data(prependable_ + initialSize)
    {}


    size_t readableBytes() const
    { return _writeIndex - _readIndex; }

    size_t writableBytes() const
    { return _data.size() - _writeIndex; }

    size_t prependableBytes() const
    { return _readIndex; }

    const char* peek()
    { return _data.data() + _readIndex; }

    void retrieve(int sz_)
    {
      assert(_readIndex + sz_ <= _writeIndex);
      _readIndex += sz_;
      if (_readIndex == _writeIndex)
        reset();
    }

    std::string retrieveAsString()
    { return retrieveAsString(readableBytes()); }

    std::string retrieveAsString(size_t sz_)
    {
      assert(sz_ <= readableBytes());
      std::string result(peek(), sz_);
      retrieve(sz_);
      return result;
    }

    void append(const char* data_, size_t len_)
    {
      extendIfNeeded(len_);
      std::copy(data_, data_ + len_, begin() + _writeIndex);
      _writeIndex += len_;
    }

    void prepend(const char* data_, size_t len_)
    {
      assert(len_ <= prependableBytes());
      _readIndex -= len_;
      std::copy(data_, data_ + len_, begin() + _readIndex);
    }

    void swap(Buffer& buf_)
    {
      using std::swap;
      swap(_data, buf_._data);
      swap(_readIndex, buf_._readIndex);
      swap(_writeIndex, buf_._writeIndex);
    }

    // TODO
    int readFd(int fd_, int* savedErrno_);

   private:

    char* begin()
    { return _data.data(); }

    void reset()
    {
      assert(_readIndex == _writeIndex);
      _readIndex = _prependable;
      _writeIndex = _prependable;
    }

    void extendIfNeeded(size_t len_)
    {
      if (len_ <= writableBytes()) return;

      // The actual "writable" bytes is prependableBytes() + writableBytes()
      else if (prependableBytes() + writableBytes() < len_ + prependSize)
      {
        // Make space by resizing
        _data.resize(_writeIndex + len_);
      }
      else
      {
        auto readableLen = readableBytes();
        std::copy(begin() + _readIndex, begin() + _writeIndex,
                  begin() + prependSize);
        _readIndex = prependSize;
        _writeIndex = _readIndex + readableBytes();
        assert(readableLen == readableBytes());
      }

    }

    const size_t _prependable;
    int _readIndex;
    int _writeIndex;
    std::vector<char> _data;
  };
}
}

#endif
