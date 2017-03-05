#include "gtest/gtest.h"

#include <ds/MinHeap.h>

#include <string>
#include <cstdlib>
#include <iterator>

// The fixture for testing class Foo.
class MinHeapTest : public ::testing::Test {
protected:
  MinHeapTest() {};
  virtual ~MinHeapTest() {};
  virtual void SetUp() {};
  virtual void TearDown() {};
};

TEST_F(MinHeapTest, testCreation)
{
  oplib::MinHeap<int> heap0;

  oplib::MinHeap<int> heap1{3, 10, 20, 5, 2, 0, 18};
  EXPECT_EQ(heap1.size(), 7u);

  // MinHeap doest not allow duplicate keys
  oplib::MinHeap<int> heap2{3, 5, 7, 11, 3, 13, 11, 18};
  EXPECT_EQ(heap2.size(), 6u);
}

TEST_F(MinHeapTest, testAccessor)
{
  oplib::MinHeap<int> heap{3, 10, 20, 5, 2, 0, 18, 3};
  EXPECT_EQ(heap.top(), 0);
  
  heap.pop();
  EXPECT_EQ(heap.top(), 2);
  heap.pop();
  EXPECT_EQ(heap.top(), 3);
  heap.pop();
  EXPECT_EQ(heap.top(), 5);
}

TEST_F(MinHeapTest, testInsert)
{
  oplib::MinHeap<int> heap{3, 10, 20, 5, 2, 0, 18, 3};
  EXPECT_EQ(heap.top(), 0);
  
  EXPECT_TRUE(heap.insert(1));
  EXPECT_EQ(heap.top(), 0);

  heap.pop();
  EXPECT_EQ(heap.top(), 1);

  // Cannot insert duplicate value
  EXPECT_FALSE(heap.insert(1));
}

