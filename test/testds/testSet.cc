#include "gtest/gtest.h"
#include <ds/Set.h>

#include <iostream>
#include <vector>
#include <iterator>

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
