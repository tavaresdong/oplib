#include "Buffer.h"
#include <assert.h>

using namespace oplib::ds;

Buffer::Buffer(int prependable_)
: _prependable(prependable_),
  _readIndex(_prependable),
  _writeIndex(_prependable),
  _data(prependable + initialSize)
{}

const char* Buffer::peek()
{
  return _data.data() + _readIndex;
}

void Buffer::reset()
{
  assert(_readIndex == _writeIndex);
  _readIndex = _prependable;
  _writeIndex = _prependable;
}

void Buffer::retrieve(int sz_)
{
  assert(_readIndex + sz_ <= _writeIndex);
  _readIndex += sz;
}

std::string Buffer::retrieveAsString(int sz_)
{
  auto sz = readableBytes() > sz_ ? sz_ : readableBytes();
  std::string ret(peek(), sz);
  retrieve(sz_);
  if (_readIndex == _writeIndex)
    reset();
  return ret;
}
