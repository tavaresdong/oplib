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
