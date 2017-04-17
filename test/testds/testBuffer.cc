#include "gtest/gtest.h"
#include <ds/Buffer.h>

#include <string>

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

  std::string str(200, 'x');
  buf.append(str);
  EXPECT_EQ(buf.readableBytes(), str.size());
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize - str.size());
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize);

  const std::string str2 = buf.retrieveAsString(50);
  EXPECT_EQ(str2.size(), 50u);
  EXPECT_EQ(buf.readableBytes(), str.size() - str2.size());
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize - str.size());
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize + str2.size());

  buf.append(str);
  EXPECT_EQ(buf.readableBytes(), 2 * str.size() - str2.size());
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize - 2 * str.size());
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize + str2.size());

  const std::string str3 = buf.retrieveAsString();
  EXPECT_EQ(str3.size(), 350u);
  EXPECT_EQ(buf.readableBytes(), 0u);
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize);
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize);
}

TEST_F(BufferTest, testBufferExpand)
{
  oplib::ds::Buffer buf;
  buf.append(std::string(400, 'y'));
  EXPECT_EQ(buf.readableBytes(), 400u);
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize - 400);

  buf.append(std::string(1000, 'x'));
  EXPECT_EQ(buf.readableBytes(), 1400u);
  EXPECT_EQ(buf.writableBytes(), 0u);
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize);

  buf.retrieveAll();
  EXPECT_EQ(buf.readableBytes(), 0u);
  EXPECT_EQ(buf.writableBytes(), 1400u);
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize);
}

TEST_F(BufferTest, testBufferMakespace)
{
  oplib::ds::Buffer buf;
  buf.append(std::string(800, 'y'));
  EXPECT_EQ(buf.readableBytes(), 800u);
  EXPECT_EQ(buf.writableBytes(), oplib::ds::Buffer::initialSize - 800);

  buf.retrieve(400);
  EXPECT_EQ(buf.readableBytes(), 400u);
  EXPECT_EQ(buf.writableBytes(), 224u);
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize + 400);

  buf.append(std::string(400, 'x'));
  EXPECT_EQ(buf.readableBytes(), 800u);
  EXPECT_EQ(buf.writableBytes(), 224u);
  EXPECT_EQ(buf.prependableBytes(), oplib::ds::Buffer::prependSize);
}
