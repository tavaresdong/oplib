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
  oplib::MinHeap<int, int> heap0;

  oplib::MinHeap<int, int> heap1{ {3, 3}, {10, 10}, {20, 20}, {5, 5}, {2, 2}, {0, 0}, {18, 18} };
  EXPECT_EQ(heap1.size(), 7u);

  // MinHeap doest not allow duplicate keys
  oplib::MinHeap<int, int> heap2{ {3, 3}, {5, 5}, {7, 7}, {11, 11}, {3, 3}, {13, 13}, {11, 11}, {18, 18} };
  EXPECT_EQ(heap2.size(), 6u);
}

TEST_F(MinHeapTest, testAccessor)
{
  oplib::MinHeap<int, int> heap{ {3, 3}, {10, 10}, {20, 20}, {5, 5}, {2, 2}, {0, 0}, {18, 18}, {3, 3} };
  EXPECT_EQ(heap.top().second, 0);
  
  heap.pop();
  EXPECT_EQ(heap.top().second, 2);
  heap.pop();
  EXPECT_EQ(heap.top().second, 3);
  heap.pop();
  EXPECT_EQ(heap.top().second, 5);
}

TEST_F(MinHeapTest, testInsert)
{
  oplib::MinHeap<int, int> heap{ {3, 3}, {10, 10}, {20, 20}, {5, 5}, {2, 2}, {0, 0}, {18, 18}, {3, 3} };
  EXPECT_EQ(heap.top().second, 0);
  
  EXPECT_TRUE(heap.insert({1, 1}));
  EXPECT_EQ(heap.top().second, 0);

  heap.pop();
  EXPECT_EQ(heap.top().second, 1);

  // Cannot insert duplicate value
  EXPECT_FALSE(heap.insert({1, 1}));
}

TEST_F(MinHeapTest, testErase)
{
  oplib::MinHeap<int, int> heap{ {10, 10}, {20, 20}, {5, 5}, {2, 2}, {0, 0}, {18, 18}, {24, 24}, {3, 3} };
  EXPECT_EQ(heap.top().second, 0);
  EXPECT_TRUE(heap.erase(3));
  EXPECT_TRUE(heap.heapified());
  heap.pop();
  heap.pop();
  EXPECT_EQ(heap.top().second, 5);
}

TEST_F(MinHeapTest, testChangeKey)
{
  oplib::MinHeap<int, int> heap{ {10, 10}, {20, 20}, {5, 5}, {2, 2}, {0, 0}, {18, 18}, {24, 24}, {3, 3} };
  EXPECT_TRUE(heap.heapified());
  EXPECT_EQ(heap.top().second, 0);

  EXPECT_TRUE(heap.changeScore(3, -1));
  EXPECT_TRUE(heap.heapified());
  EXPECT_EQ(heap.size(), 8u);
  EXPECT_EQ(heap.top().second, -1);
}
