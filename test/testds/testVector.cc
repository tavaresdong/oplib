#include "gtest/gtest.h"
#include <ds/Vector.H>
#include <string>
#include <iostream>
#include <utility>
#include <list>
#include <vector>
#include <stdexcept>
#include <limits>

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

TEST_F(VectorTest, testMove)
{
  oplib::ds::Vector<int> v { 1, 2 };
  oplib::ds::Vector<int> vm(std::move(v));
  EXPECT_EQ(vm.size(), 2u);
  EXPECT_EQ(v.size(), 0u);
  EXPECT_EQ(vm[0], 1);
  EXPECT_EQ(vm[1], 2);

  v = std::move(vm);
  EXPECT_EQ(v.size(), 2u);
  EXPECT_EQ(vm.size(), 0u);
}

TEST_F(VectorTest, testRangeConstruction)
{
  {
    std::list<int> l { 1, 2, 3 };
    oplib::ds::Vector<int> v(l.begin(), l.end());
    EXPECT_EQ(v.size(), 3u);
  }

  {
    std::vector<int> stdv { 1, 2, 3 };
    oplib::ds::Vector<int> v(stdv.begin(), stdv.end());
    EXPECT_EQ(v.size(), 3u);
  }
}

TEST_F(VectorTest, testReverseIterator)
{
  {
    oplib::ds::Vector<int> v;

    v.push_back(1);
    v.push_back(2);
    auto ri = v.rbegin();
    EXPECT_EQ(*ri, 2);
    ++ri;
    EXPECT_EQ(*ri, 1);
    ++ri;
    EXPECT_EQ(ri, v.rend());
  }

  {
    // Also tested initializer_list-construction
    const oplib::ds::Vector<int> v { 1, 2 };

    auto ri = v.crbegin();
    EXPECT_EQ(*ri, 2);
    ++ri;
    EXPECT_EQ(*ri, 1);
    ++ri;
    EXPECT_EQ(ri, v.crend());
  }
}

TEST_F(VectorTest, testAt)
{
  oplib::ds::Vector<int> v { 1, 2 };
  EXPECT_THROW(v.at(5), std::range_error);
}

TEST_F(VectorTest, testMaxSize)
{
  oplib::ds::Vector<int> v;
  EXPECT_EQ(v.max_size(), std::numeric_limits<size_t>::max());
}

namespace
{
  class Movable
  {
   public:
    Movable(int a, int b) : i(a * b) {}
    Movable() {}
    ~Movable() {}
    Movable(const Movable& m) : i(m.i)
    {
      std::cout << "Copy ctor called" << std::endl;
    }

    Movable& operator = (const Movable& m)
    {
      i = m.i;
      return *this;
    }

    Movable(Movable&& m) : i(m.i) 
    { 
      std::cout << "Move ctor called" << std::endl;
      m.i = 0; 
    }
    Movable& operator = (Movable&& m)
    {
      i = m.i;
      m.i = 0;
      return *this;
    }
    
    int get() { return i; }
   private:
    int i = 1;
  };
}

TEST_F(VectorTest, testEmplaceBack)
{
  oplib::ds::Vector<Movable> v;
  Movable m;
  EXPECT_EQ(m.get(), 1);
  v.emplace_back(std::move(m));
  EXPECT_EQ(v.size(), 1u);
  EXPECT_EQ(m.get(), 0);
  EXPECT_EQ(v.front().get(), 1);

  v.emplace_back(3, 4);
  EXPECT_EQ(v.size(), 2u);
  EXPECT_EQ(v.back().get(), 12);
}
