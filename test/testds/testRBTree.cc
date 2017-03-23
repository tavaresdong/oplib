#include "gtest/gtest.h"
#include <ds/RBTree.H>
#include <string>
#include <cstdlib>
#include <iterator>
#include <functional>
#include <iostream>

namespace 
{
class KeyOfValue
{
public:
  int operator() (int v)
  { return v; }
};
}

// The fixture for testing class Foo.
class RBTreeTest : public ::testing::Test {
protected:
  RBTreeTest();
  virtual ~RBTreeTest();
  virtual void SetUp();
  virtual void TearDown();

  oplib::ds::RBTree<int, int, KeyOfValue, std::less<int>> rbtree;
};

RBTreeTest::RBTreeTest() {}

RBTreeTest::~RBTreeTest() {}

void RBTreeTest::SetUp() 
{
  EXPECT_TRUE(rbtree.empty());
  EXPECT_TRUE(rbtree.rbPropertyKept());
  EXPECT_EQ(rbtree.size(), 0u);

  rbtree.insertUnique(10); EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(7);  EXPECT_TRUE(rbtree.rbPropertyKept()); 
  rbtree.insertUnique(8);  EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(15); EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(5);  EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(6);  EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(11); EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(13); EXPECT_TRUE(rbtree.rbPropertyKept());
  rbtree.insertUnique(12); EXPECT_TRUE(rbtree.rbPropertyKept());
}

void RBTreeTest::TearDown() 
{
  rbtree.clear();
  EXPECT_TRUE(rbtree.rbPropertyKept());
}

TEST_F(RBTreeTest, testInsertion)
{
  EXPECT_FALSE(rbtree.insertUnique(12).second);
  EXPECT_TRUE(rbtree.insertUnique(16).second);
  EXPECT_EQ(rbtree.size(), 10u);

  auto first = rbtree.begin();
  auto second = first;
  ++second;
  for (; second != rbtree.end(); ++first, ++second)
  {
    EXPECT_TRUE(*first < *second);
  }
}

TEST_F(RBTreeTest, testFind)
{
  EXPECT_TRUE(rbtree.find(-1) == rbtree.end());
  EXPECT_TRUE(rbtree.find(9) == rbtree.end());
  EXPECT_TRUE(rbtree.find(19) == rbtree.end());
  EXPECT_TRUE(rbtree.find(5) != rbtree.end());
  EXPECT_EQ(5, *(rbtree.find(5)));
}

TEST_F(RBTreeTest, testErase)
{
  auto p = rbtree.erase(5);
  EXPECT_TRUE(p.second);
  EXPECT_EQ(*(p.first), 6);
  EXPECT_EQ(rbtree.size(), 8u);
  EXPECT_TRUE(rbtree.rbPropertyKept());

  p = rbtree.erase(6);
  EXPECT_TRUE(p.second);
  EXPECT_EQ(*(p.first), 7);
  EXPECT_EQ(rbtree.size(), 7u);
  EXPECT_TRUE(rbtree.rbPropertyKept());

  p = rbtree.erase(7);
  EXPECT_TRUE(p.second);
  EXPECT_EQ(*(p.first), 8);
  EXPECT_EQ(rbtree.size(), 6u);
  EXPECT_TRUE(rbtree.rbPropertyKept());
}
