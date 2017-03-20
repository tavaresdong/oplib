#include "gtest/gtest.h"
#include <ds/RBTree.H>
#include <string>
#include <cstdlib>
#include <iterator>

namespace 
{
class Comparator
{
  bool operator() (int v1, int v2)
  { return v1 < v2; };
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
  auto kofv = [](int i ) { return i; };
  oplib::ds::RBTree<int, int, decltype(kofv), Comparator>  rbtree;
  EXPECT_TRUE(rbtree.empty());
  EXPECT_EQ(rbtree.size(), 0u);
}

