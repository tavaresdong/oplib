#include "alloc.H"
#include "gtest/gtest.h"
#include <ds/Linkedlist.H>
#include <string>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <iterator>

// The fixture for testing class Foo.
class LinkedlistTest : public ::testing::Test {
protected:
  LinkedlistTest();
  virtual ~LinkedlistTest();
  virtual void SetUp();
  virtual void TearDown();
};

LinkedlistTest::LinkedlistTest() {}

LinkedlistTest::~LinkedlistTest() {}

void LinkedlistTest::SetUp() {}

void LinkedlistTest::TearDown() {}

TEST_F(LinkedlistTest, testBasicOperation)
{
  oplib::Linkedlist<int> v;
  ASSERT_TRUE(v.empty());
  v.push_back(0);
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(4); 
  v.push_back(5);

  ASSERT_FALSE(v.empty());
  ASSERT_EQ(v.size(), (size_t) 6);

  int i = 0;
  for (auto iter = v.begin(); iter != v.end(); ++iter, ++i)
  {
      ASSERT_EQ(*iter, i);
  }

  ASSERT_EQ(v.front(), 0);
  ASSERT_EQ(v.back(), 5);

  v.push_front(-1);
  ASSERT_EQ(v.size(), (size_t) 7);
  ASSERT_EQ(v.front(), -1);
}

TEST_F(LinkedlistTest, testElementManipulation)
{
  oplib::Linkedlist<int> v;
  ASSERT_TRUE(v.empty());
  v.push_back(0);
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  
  auto iter = v.begin();
  ++iter;
  iter = v.erase(iter);
  ASSERT_EQ(*iter, 2);
  --iter;
  ASSERT_EQ(*iter, 0);

  v.pop_back();
  ASSERT_EQ(v.back(), 2);
  
  v.pop_front();
  ASSERT_EQ(v.front(), 2);

  v.push_front(3);
  v.push_back(3);
  ASSERT_EQ(v.size(), (size_t) 3);

  v.clear();
  ASSERT_TRUE(v.empty());

  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(2);
  v.remove(2);
  ASSERT_EQ(v.size(), (size_t) 2);
  
  v.push_back(2);
  v.push_back(3);
  v.remove_if([](int a){ return a == 3; });
  ASSERT_EQ(v.size(), (size_t) 2);
  
  v.push_front(1);
  v.push_back(1);
  v.push_back(1);
  v.push_back(1);
  v.unique();
  ASSERT_EQ(v.size(), (size_t) 3);
  ASSERT_EQ(v.front(), 1);
  ASSERT_EQ(v.back(), 1);
}

TEST_F(LinkedlistTest, testRotate)
{
  oplib::Linkedlist<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(4);
  v.push_back(5);

  v.rotate(7);
  auto iter = v.begin();
  ASSERT_EQ(*iter, 4);
  ++iter;
  ASSERT_EQ(*iter, 5);
  ++iter;
  ASSERT_EQ(*iter, 1);

  v.rotate(3);
  ASSERT_EQ(*v.begin(), 1);
}

TEST_F(LinkedlistTest, testComplexOperation)
{
  oplib::Linkedlist<int> v;
  ASSERT_TRUE(v.empty());
  v.push_back(1);
  v.push_back(2);
  
  oplib::Linkedlist<int> v2;
  v2.push_back(3);
  v2.push_back(4);
  v.splice(v.end(), v2);
  ASSERT_EQ(v.size(), (size_t) 4);
  
  int i = 1;
  for (auto val : v)
  {
      ASSERT_EQ(val, i);
      ++i;
  }

  v.reverse();
  i = 4;
  for (auto val : v)
  {
      ASSERT_EQ(val, i);
      --i;
  }

  // Test reverse for a range 4 [3 2] 1 -> 4 2 3 1 
  auto iter = std::next(v.begin());
  v.reverse(iter, std::prev(v.end()));
  iter = v.begin();
  ASSERT_EQ(*iter, 4);
  ++iter;
  ASSERT_EQ(*iter, 2);
  ++iter;
  ASSERT_EQ(*iter, 3);
  ++iter;
  ASSERT_EQ(*iter, 1);
}

TEST_F(LinkedlistTest, testMergeAndSort)
{
  oplib::Linkedlist<int> l;
  l.push_back(1); 
  l.push_back(3);
  l.push_back(7);
  oplib::Linkedlist<int> l2;
  l2.push_back(2);
  l2.push_back(4);
  l2.push_back(5);
  l2.push_back(6);
  l2.push_back(8);
  l.merge(l2);
  ASSERT_TRUE(l2.empty());

  int i = 1;
  for (auto val : l)
  {
      ASSERT_EQ(val, i++);
  }

  oplib::Linkedlist<int> tosort;   
  srand(std::time(0));
  for (auto i = 0; i < 100; i++)
  {
      tosort.push_back(std::rand() % 100);
  }
  tosort.sort();
  int pre = INT_MIN;
  for (auto val : tosort)
  {
      ASSERT_TRUE(pre <= val);
      pre = val;
  }
}
