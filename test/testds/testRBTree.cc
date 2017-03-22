#include "gtest/gtest.h"
#include <ds/RBTree.H>
#include <string>
#include <cstdlib>
#include <iterator>
#include <functional>
#include <iostream>

namespace 
{
class Comparator
{
public:
  bool operator() (int v1, int v2)
  { return v1 < v2; };
};

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
};

RBTreeTest::RBTreeTest() {}

RBTreeTest::~RBTreeTest() {}

void RBTreeTest::SetUp() {}

void RBTreeTest::TearDown() {}

TEST_F(RBTreeTest, testCreation)
{
  oplib::ds::RBTree<int, int, KeyOfValue, Comparator> rbtree;
  EXPECT_TRUE(rbtree.empty());
  EXPECT_EQ(rbtree.size(), 0u);
}

TEST_F(RBTreeTest, testInsertion)
{
  oplib::ds::RBTree<int, int, KeyOfValue, std::less<int>> rbtree;
  EXPECT_TRUE(rbtree.empty());
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

  EXPECT_FALSE(rbtree.insertUnique(12).second);
  EXPECT_EQ(rbtree.size(), 9u);

  auto first = rbtree.begin();
  auto second = first;
  ++second;
  for (; second != rbtree.end(); ++first, ++second)
  {
    EXPECT_TRUE(*first < *second);
  }
}

