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

TEST_F(MinHeapTest, testBasicOperation)
{
  oplib::MinHeap<int> heap;

  oplib::MinHeap<int> heapInit{3, 10, 20, 5, 2, 0, 18};
  EXPECT_EQ(heapInit.top(), 0);
  
  heapInit.pop();
  EXPECT_EQ(heapInit.top(), 2);
  heapInit.pop();
  EXPECT_EQ(heapInit.top(), 3);
  heapInit.pop();
  EXPECT_EQ(heapInit.top(), 5);
}

