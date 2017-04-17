#include "gtest/gtest.h"
#include <ds/Buffer.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <functional>

class BufferTest : public ::testing::Test 
{
protected:
  BufferTest() {};
  virtual ~BufferTest() {};
  virtual void SetUp() {};
  virtual void TearDown() {};
};

TEST_F(BufferTest, testAppendRetrieve)
{
  oplib::ds::Buffer buf;
  EXPECT_EQ(buf.readableBytes(), 0u);
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize);
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize);
}
