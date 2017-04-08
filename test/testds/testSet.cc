#include "gtest/gtest.h"
#include <ds/Set.h>
#include <vector>

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
