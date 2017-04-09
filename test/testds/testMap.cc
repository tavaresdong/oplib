#include "gtest/gtest.h"
#include <ds/Map.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>

class MapTest : public ::testing::Test 
{
protected:
  MapTest() {};
  virtual ~MapTest() {};
  virtual void SetUp() {};
  virtual void TearDown() {};
};

TEST_F(MapTest, testConstruction)
{
  oplib::ds::Map<int, int> m;
  EXPECT_EQ(m.size(), 0u);
}

TEST_F(MapTest, testRangeConstruction)
{
  std::map<int, int> stdmap { {4, 4}, {1, 1}, {3, 7}, {34, 22}, {7, 11} };
  oplib::ds::Map<int, int> m(stdmap.begin(), stdmap.end());
  EXPECT_EQ(m.size(), 5u);
}

TEST_F(MapTest, testAccessors)
{
  std::map<int, int> stdmap { {4, 4}, {1, 1}, {3, 7}, {34, 22}, {7, 11} };
  oplib::ds::Map<int, int> m(stdmap.begin(), stdmap.end());
  EXPECT_EQ(m.size(), 5u);
  EXPECT_FALSE(m.empty());
}

TEST_F(MapTest, testBeginEnd)
{
  std::map<int, int> stdmap { {4, 4}, {1, 1}, {3, 7}, {34, 22}, {7, 11} };
  oplib::ds::Map<int, int> m(stdmap.begin(), stdmap.end());
  auto ib = m.begin();
  auto ie = m.end();
  int pre = 0;
  for (; ib != ie; ++ib)
  {
    EXPECT_TRUE(pre < ib->first);
    pre = ib->first;
  }
}

TEST_F(MapTest, testKeyComp)
{
  oplib::ds::Map<int, int> m;
  auto keycomp = m.key_comp();
  EXPECT_TRUE(keycomp(1, 2));
  EXPECT_FALSE(keycomp(2, 1));
  EXPECT_FALSE(keycomp(1, 1));
}

TEST_F(MapTest, testValueComp)
{
  oplib::ds::Map<int, int> m;
  auto valuecomp = m.value_comp();
  EXPECT_TRUE(valuecomp(std::pair<int, int>(1, 1), std::pair<int, int>(2, 2)));
  EXPECT_FALSE(valuecomp(std::pair<int, int>(2, 2), std::pair<int, int>(1, 1)));
  EXPECT_FALSE(valuecomp(std::pair<int, int>(1, 1), std::pair<int, int>(1, 1)));
}

TEST_F(MapTest, testInsert)
{
  oplib::ds::Map<int, int> m;
  EXPECT_TRUE(m.insert(std::make_pair(3, 3)).second);
  EXPECT_TRUE(m.insert(std::make_pair(1, 1)).second);
  EXPECT_FALSE(m.insert(std::make_pair(1, 1)).second);
}

TEST_F(MapTest, testRangeInsert)
{
  oplib::ds::Map<int, int> m;
  std::map<int, int> stdmap { {4, 4}, {1, 1}, {3, 7}, {34, 22}, {7, 11}, {1, 2} };
  m.insert(stdmap.begin(), stdmap.end());
  EXPECT_EQ(m.size(), 5u);
}

TEST_F(MapTest, testHintInsert)
{
  oplib::ds::Map<int, int> m { {3, 3}, {2, 2}, {1, 1}, {5, 5} };
  auto iter = m.cbegin();
  std::advance(iter, 3);
  EXPECT_EQ(iter->first, 5);
  iter = m.insert(iter, {4, 4} );
  EXPECT_EQ(iter->first, 4);
}

TEST_F(MapTest, testErase)
{
  oplib::ds::Map<int, int> m { {3, 3}, {2, 2}, {1, 1}, {5, 5} };
  EXPECT_TRUE(m.erase(3) == 1);
  EXPECT_EQ(m.size(), 3u);
  EXPECT_TRUE(m.erase(6) == 0);
}

TEST_F(MapTest, testEraseByPos)
{
  oplib::ds::Map<int, int> m { {3, 3}, {2, 2}, {1, 1}, {5, 5} };
  auto iter = m.begin();
  std::advance(iter, 2);
  EXPECT_EQ(iter->second, 3);
  iter = m.erase(iter);
  EXPECT_EQ(iter->second, 5); 
}

TEST_F(MapTest, testRangeErase)
{
  oplib::ds::Map<int, int> m { {3, 3}, {2, 2}, {1, 1}, {5, 5} };

  auto first = m.begin();
  auto last = m.begin();
  std::advance(first, 1);
  std::advance(last, 3);
  EXPECT_EQ(first->second, 2);
  EXPECT_EQ(last->second, 5);
  auto it = m.erase(first, last);
  EXPECT_TRUE(it == last);
  EXPECT_EQ(it->second, 5);
  EXPECT_EQ(m.size(), 2u);
}

TEST_F(MapTest, testClear)
{
  oplib::ds::Map<int, int> m { {3, 3}, {2, 2}, {1, 1}, {5, 5} };
  EXPECT_EQ(m.size(), 4u);
  m.clear();
  EXPECT_TRUE(m.empty());
}

TEST_F(MapTest, testSwap)
{
  oplib::ds::Map<int, int> m1 { {3, 3}, {2, 2}, {1, 1}, {5, 5} };

  oplib::ds::Map<int, int> m2;
  m2.insert({3, 3});
  using std::swap;
  swap(m1, m2);
  EXPECT_EQ(m1.size(), 1u);
  EXPECT_EQ(m2.size(), 4u);
  m1.clear();
  EXPECT_EQ(m2.size(), 4u);
}

TEST_F(MapTest, testFindCount)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };

  auto it = m.find(3);
  EXPECT_EQ(it->second, 30);

  EXPECT_TRUE(m.find(10) == m.end());

  EXPECT_EQ(m.count(3), 1u);
  EXPECT_EQ(m.count(20), 0u);
}

TEST_F(MapTest, testLowerBound)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };

  auto it = m.lower_bound(-1);
  EXPECT_EQ(it->second, 1);
  it = m.lower_bound(2);
  EXPECT_EQ(it->second, 2);
  it = m.lower_bound(4);
  EXPECT_EQ(it->second, 5);
  it = m.lower_bound(6);
  EXPECT_TRUE(it == m.end());
}

TEST_F(MapTest, testUpperBound)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };

  auto it = m.upper_bound(-1);
  EXPECT_EQ(it->second, 1);
  it = m.upper_bound(3);
  EXPECT_EQ(it->second, 5);
  it = m.upper_bound(4);
  EXPECT_EQ(it->second, 5);
  it = m.upper_bound(5);
  EXPECT_TRUE(it == m.end());
}

TEST_F(MapTest, testEqualRange)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };

  auto range = m.equal_range(1);
  EXPECT_EQ(range.first->second, 1);
  EXPECT_EQ(range.second->second, 2);

  range = m.equal_range(0);
  EXPECT_EQ(range.first->second, 1);
  EXPECT_EQ(range.second->second, 1);
}

TEST_F(MapTest, testCopyAssign)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };
  oplib::ds::Map<int, int> m2(m);
  EXPECT_EQ(m2.size(), 4u);

  oplib::ds::Map<int, int> m3;
  m3 = m2;
  EXPECT_EQ(m3.size(), 4u);
}

TEST_F(MapTest, testMoveAssign)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };
  oplib::ds::Map<int, int> m2(std::move(m));
  EXPECT_EQ(m2.size(), 4u);
  EXPECT_EQ(m.size(), 0u);

  oplib::ds::Map<int, int> m3;
  m3 = std::move(m2);
  EXPECT_EQ(m3.size(), 4u);
  EXPECT_EQ(m2.size(), 0u);
}

TEST_F(MapTest, testReverseIter)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };
  auto rit = m.rbegin();
  EXPECT_EQ(rit->second, 5);
}

TEST_F(MapTest, testSubscript)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };
  auto val = m[3];
  EXPECT_EQ(val, 30);
  EXPECT_EQ(m.size(), 4u);
  EXPECT_EQ(m[4], 0);
  EXPECT_EQ(m.size(), 5u);
  m[4] = 4;
  EXPECT_EQ(m[4], 4);
  EXPECT_EQ(m.size(), 5u);
}

TEST_F(MapTest, testAt)
{
  oplib::ds::Map<int, int> m { {3, 30}, {2, 2}, {1, 1}, {5, 5} };
  auto val = m.at(3);
  EXPECT_EQ(val, 30);
  EXPECT_EQ(m.size(), 4u);
  EXPECT_THROW(m.at(4), std::out_of_range);
  EXPECT_EQ(m.size(), 4u);
}
