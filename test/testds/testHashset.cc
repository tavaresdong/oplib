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
