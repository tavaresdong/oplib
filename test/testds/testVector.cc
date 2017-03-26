#include "alloc.H"
#include "gtest/gtest.h"
#include <ds/Vector.H>
#include <string>
#include <iostream>
#include <utility>

// The fixture for testing class Foo.
class VectorTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    VectorTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~VectorTest();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();
};

VectorTest::VectorTest() {}

VectorTest::~VectorTest() {}

void VectorTest::SetUp() {}

void VectorTest::TearDown() {}

TEST_F(VectorTest, testAccessor) 
{
  {
    oplib::ds::Vector<int> v(5, 3);
    EXPECT_EQ(v.size(), (size_t) 5);
    v.push_back(42);
    EXPECT_EQ(v.size(), (size_t) 6);
    EXPECT_EQ(v.back(), 42);
    EXPECT_EQ(v.front(), 3);
    EXPECT_EQ(*v.begin(), 3);
    *v.begin() = 4;
    EXPECT_EQ(*v.begin(), 4);
    v.pop_back();
    EXPECT_EQ(v.size(), (size_t) 5);
  }

  {
    oplib::ds::Vector<int> v(10);
    EXPECT_EQ(v.size(), (size_t) 10);
    EXPECT_EQ(v.front(), 0);
    EXPECT_FALSE(v.empty());
  }
}

TEST_F(VectorTest, testCopyAssign)
{
  oplib::ds::Vector<std::string> v(5, "good");
  oplib::ds::Vector<std::string> vc(v);
  EXPECT_EQ(vc.size(), (size_t) 5);
  EXPECT_EQ(vc.back(), "good");
  oplib::ds::Vector<std::string> vassign;
  vassign = v;
  EXPECT_EQ(vassign.size(), (size_t) 5);
  EXPECT_EQ(vassign.back(), "good");
}

TEST_F(VectorTest, testElementOp)
{
  oplib::ds::Vector<int> v;
  v.push_back(0);
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(4); 
  v.push_back(5);
  int i = 0;
  for (auto iter = v.begin(); iter != v.end(); iter++, i++)
  {
    EXPECT_EQ(*iter, i);
  }
  EXPECT_EQ(v[1], 1);
  v[1] = 5;
  EXPECT_EQ(v[1], 5);
  auto iter = v.begin() + 3;
  v.erase(iter);
  EXPECT_EQ(v[3], 4);
  EXPECT_EQ(v.size(), (size_t) 5);
  v.resize(4, 1);
  EXPECT_EQ(v.size(), (size_t) 4);
  v.resize(6, 1);
  EXPECT_EQ(v.size(), (size_t) 6);
  EXPECT_EQ(v[5], 1);
  v.insert(v.begin(), 2, 10);
  EXPECT_EQ(v.size(), (size_t) 8);
  EXPECT_EQ(v[0], v[1]);
  EXPECT_EQ(v[0], 10);
  v.clear();
  EXPECT_EQ(v.size(), (size_t) 0);
}

TEST_F(VectorTest, testSizeOfVector)
{
  oplib::ds::Vector<int> v;
  EXPECT_EQ(sizeof(v), (size_t) 24);
}

TEST_F(VectorTest, testSwap)
{
  oplib::ds::Vector<int> v(2, 3);
  oplib::ds::Vector<int> v2(3, 4);

  // Note: let the compiler choose the right overload
  using std::swap;
  swap(v, v2);
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(v2.size(), 2u);
  EXPECT_EQ(v.front(), 4);
  EXPECT_EQ(v2.front(), 3);
}

TEST_F(VectorTest, testShrinkToFit)
{
  oplib::ds::Vector<int> v;

  v.reserve(100);
  v.push_back(1);
  v.push_back(2);

  EXPECT_EQ(v.size(), 2u);
  EXPECT_EQ(v.capacity(), 100u);

  v.shrink_to_fit();
  EXPECT_EQ(v.size(), 2u);
  EXPECT_EQ(v.capacity(), 2u);
}
