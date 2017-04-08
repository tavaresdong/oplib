#include "gtest/gtest.h"
#include <ds/Set.h>

#include <iostream>
#include <vector>
#include <iterator>
#include <utility>

class SetTest : public ::testing::Test 
{
protected:
  SetTest() {};
  virtual ~SetTest() {};
  virtual void SetUp() {};
  virtual void TearDown() {};
};

TEST_F(SetTest, testConstruction)
{
  oplib::ds::Set<int> set;
  EXPECT_EQ(set.size(), 0u);
}

TEST_F(SetTest, testRangeConstruction)
{
  std::vector<int> vec { 4, 1 ,3 ,34, 7 };
  oplib::ds::Set<int> set(vec.begin(), vec.end());
  EXPECT_EQ(set.size(), 5u);
}

TEST_F(SetTest, testAccessors)
{
  std::vector<int> vec { 4, 1 ,3 ,34, 7 };
  oplib::ds::Set<int> set(vec.begin(), vec.end());
  EXPECT_EQ(set.size(), 5u);
  EXPECT_FALSE(set.empty());
}

TEST_F(SetTest, testBeginEnd)
{
  std::vector<int> vec { 4, 1 ,3 ,34, 7 };
  oplib::ds::Set<int> set(vec.begin(), vec.end());
  auto ib = set.begin();
  auto ie = set.end();
  int pre = 0;
  for (; ib != ie; ++ib)
  {
    EXPECT_TRUE(pre < *ib);
    pre = *ib;
  }
}

TEST_F(SetTest, testKeyComp)
{
  oplib::ds::Set<int> set;
  auto keycomp = set.key_comp();
  EXPECT_TRUE(keycomp(1, 2));
  EXPECT_FALSE(keycomp(2, 1));
  EXPECT_FALSE(keycomp(1, 1));
}

TEST_F(SetTest, testInsert)
{
  oplib::ds::Set<int> set;
  EXPECT_TRUE(set.insert(3).second);
  EXPECT_TRUE(set.insert(1).second);
  EXPECT_FALSE(set.insert(1).second);
}

TEST_F(SetTest, testRangeInsert)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 4, 3 };
  set.insert(vec.begin(), vec.end());
  for (auto iter = set.begin(); iter != set.end(); ++iter)
    std::cout << *iter << " ";
  EXPECT_EQ(set.size(), 4u);
}

TEST_F(SetTest, testHintInsert)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5};
  set.insert(vec.begin(), vec.end());
  auto iter = set.cbegin();
  std::advance(iter, 3);
  EXPECT_EQ(*iter, 5);
  iter = set.insert(iter, 4);
  EXPECT_EQ(*iter, 4);
}

TEST_F(SetTest, testErase)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5};
  set.insert(vec.begin(), vec.end());
  EXPECT_TRUE(set.erase(3) == 1);
  EXPECT_EQ(set.size(), 3u);
  EXPECT_TRUE(set.erase(6) == 0);
}

TEST_F(SetTest, testEraseByPos)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5};
  set.insert(vec.begin(), vec.end());
  auto iter = set.begin();
  std::advance(iter, 2);
  EXPECT_EQ(*iter, 3);
  iter = set.erase(iter);
  EXPECT_EQ(*iter, 5); 
}

TEST_F(SetTest, testRangeErase)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5};
  set.insert(vec.begin(), vec.end());

  auto first = set.begin();
  auto last = set.begin();
  std::advance(first, 1);
  std::advance(last, 3);
  EXPECT_EQ(*first, 2);
  EXPECT_EQ(*last, 5);
  auto it = set.erase(first, last);
  EXPECT_TRUE(it == last);
  EXPECT_EQ(*it, 5);
  EXPECT_EQ(set.size(), 2u);
}

TEST_F(SetTest, testClear)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5};
  set.insert(vec.begin(), vec.end());
  EXPECT_EQ(set.size(), 4u);
  set.clear();
  EXPECT_TRUE(set.empty());
}

TEST_F(SetTest, TestSwap)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5 };
  set.insert(vec.begin(), vec.end());

  oplib::ds::Set<int> set2;
  set2.insert(3);
  using std::swap;
  swap(set, set2);
  EXPECT_EQ(set.size(), 1u);
  EXPECT_EQ(set2.size(), 4u);
  set.clear();
  EXPECT_EQ(set2.size(), 4u);
}

TEST_F(SetTest, TestFindCount)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5 };
  set.insert(vec.begin(), vec.end());

  auto it = set.find(3);
  EXPECT_EQ(*it, 3);

  it = set.find(10);
  EXPECT_TRUE(it == set.end());

  auto cnt = set.count(3);
  EXPECT_EQ(cnt, 1u);
  
  cnt = set.count(20);
  EXPECT_EQ(cnt, 0u);
}

TEST_F(SetTest, TestLowerBound)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5 };
  set.insert(vec.begin(), vec.end());

  auto it = set.lower_bound(-1);
  EXPECT_EQ(*it, 1);
  it = set.lower_bound(2);
  EXPECT_EQ(*it, 2);
  it = set.lower_bound(4);
  EXPECT_EQ(*it, 5);
  it = set.lower_bound(6);
  EXPECT_TRUE(it == set.end());
}

TEST_F(SetTest, TestUpperBound)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5 };
  set.insert(vec.begin(), vec.end());

  auto it = set.upper_bound(-1);
  EXPECT_EQ(*it, 1);
  it = set.upper_bound(3);
  EXPECT_EQ(*it, 5);
  it = set.upper_bound(4);
  EXPECT_EQ(*it, 5);
  it = set.upper_bound(5);
  EXPECT_TRUE(it == set.end());
}

TEST_F(SetTest, TestEqualRange)
{
  oplib::ds::Set<int> set;
  std::vector<int> vec { 3, 2, 1, 5 };
  set.insert(vec.begin(), vec.end());

  auto range = set.equal_range(1);
  EXPECT_EQ(*range.first, 1);
  EXPECT_EQ(*range.second, 2);

  range = set.equal_range(0);
  EXPECT_EQ(*range.first, 1);
  EXPECT_EQ(*range.second, 1);
}
