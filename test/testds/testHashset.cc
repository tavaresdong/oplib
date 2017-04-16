#include "gtest/gtest.h"
#include <ds/Hashset.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <functional>

class HashsetTest : public ::testing::Test 
{
protected:
  HashsetTest() {};
  virtual ~HashsetTest() {};
  virtual void SetUp() {};
  virtual void TearDown() {};
};

namespace
{
  template <typename T>
  struct Identity
  {
    T operator()(const T& val_) const
    { return val_; }
  };
}

TEST_F(HashsetTest, testConstruction)
{
  oplib::ds::Hashset<int> iht(50);
  EXPECT_EQ(iht.size(), 0u);
}

TEST_F(HashsetTest, testInsert)
{
  oplib::ds::Hashset<int> iht(50);
  EXPECT_EQ(iht.size(), 0u);
  auto it = iht.insert(5);
  EXPECT_TRUE(it.second);
  EXPECT_EQ(*it.first, 5);

  it = iht.insert(55);
  EXPECT_TRUE(it.second);
  EXPECT_EQ(*it.first, 55);

  it = iht.insert(5);
  EXPECT_FALSE(it.second);
  EXPECT_EQ(*it.first, 5);
}

TEST_F(HashsetTest, testHintInsert)
{
  oplib::ds::Hashset<int> iht(50);
  EXPECT_EQ(iht.size(), 0u);
  iht.insert(5);
  iht.insert(55);

  auto iter = iht.find(5);
  iter = iht.insert(iter, 5);
  EXPECT_EQ(*iter, 5);
  EXPECT_EQ(iht.size(), 2u);

  iter = iht.insert(iter, 20);
  EXPECT_EQ(*iter, 20);
  EXPECT_EQ(iht.size(), 3u);
}

TEST_F(HashsetTest, testBeginEnd)
{
  oplib::ds::Hashset<int> iht(50);
  EXPECT_EQ(iht.size(), 0u);
  iht.insert(5);
  iht.insert(55);
  iht.insert(5);
  EXPECT_EQ(iht.size(), 2u);
  iht.insert(14);
  iht.insert(19);
  iht.insert(28);
  for (auto iter = iht.begin(); iter != iht.end(); ++iter)
  { std::cout << *iter << std::endl; }
}

TEST_F(HashsetTest, testFindCount)
{
  oplib::ds::Hashset<int> iht(50);
  iht.insert(5);
  iht.insert(55);
  iht.insert(48);
  iht.insert(21);

  auto iter = iht.find(5);
  EXPECT_EQ(*iter, 5);
  
  iter = iht.find(22);
  EXPECT_TRUE(iter == iht.end());

  EXPECT_EQ(iht.count(21), 1u);
  EXPECT_EQ(iht.count(22), 0u);
}

TEST_F(HashsetTest, testEraseByPosition)
{
  oplib::ds::Hashset<int> iht(50);
  iht.insert(5);
  iht.insert(55);

  auto iter = iht.find(55);
  iter = iht.erase(iter);
  EXPECT_EQ(*iter, 5);
}

TEST_F(HashsetTest, testEraseByKey)
{
  oplib::ds::Hashset<int> iht(50);
  iht.insert(5);
  iht.insert(55);

  EXPECT_EQ(iht.erase(5), 1u);
  EXPECT_EQ(iht.erase(53), 0u);
}

TEST_F(HashsetTest, testEraseByRange)
{
  oplib::ds::Hashset<int> iht(50);
  iht.insert(5);
  iht.insert(55);

  EXPECT_EQ(iht.size(), 2u);

  auto iter = iht.erase(iht.begin(), iht.end());
  EXPECT_EQ(iht.size(), 0u);
  EXPECT_EQ(iter, iht.end());
}

TEST_F(HashsetTest, testClear)
{
  oplib::ds::Hashset<int> iht(50);
  iht.insert(5);
  iht.insert(55);

  EXPECT_EQ(iht.size(), 2u);

  iht.clear();
  EXPECT_EQ(iht.size(), 0u);
}

TEST_F(HashsetTest, testSwap)
{
  oplib::ds::Hashset<int> iht(50);
  iht.insert(5);
  iht.insert(55);

  oplib::ds::Hashset<int> iht2(50);
  iht2.insert(3);

  using std::swap;
  swap(iht, iht2);
  EXPECT_EQ(iht.size(), 1u);
}

