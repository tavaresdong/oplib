#include "gtest/gtest.h"
#include <ds/Hashtable.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <functional>

class HashtableTest : public ::testing::Test 
{
protected:
  HashtableTest() {};
  virtual ~HashtableTest() {};
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

TEST_F(HashtableTest, testConstruction)
{
  oplib::ds::Hashtable<int, int, Identity<int>, Identity<int>, std::equal_to<int>> iht(50);
  EXPECT_EQ(iht.size(), 0u);
}

TEST_F(HashtableTest, testInsert)
{
  oplib::ds::Hashtable<int, int, Identity<int>, Identity<int>, std::equal_to<int>> iht(50);
  EXPECT_EQ(iht.size(), 0u);
  EXPECT_TRUE(iht.insertUnique(5));
  EXPECT_TRUE(iht.insertUnique(55));
  EXPECT_FALSE(iht.insertUnique(5));
}
