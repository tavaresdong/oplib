#include "alloc.H"
#include "gtest/gtest.h"
#include "vector.H"
#include <string>
#include <iostream>

// The fixture for testing class Foo.
class vectorTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    vectorTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~vectorTest();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();
};

vectorTest::vectorTest() {}

vectorTest::~vectorTest() {}

void vectorTest::SetUp() {}

void vectorTest::TearDown() {}

TEST_F(vectorTest, basic_operation_test) 
{
    {
        oplib::Vector<int> v(5, 3);
        ASSERT_EQ(v.size(), (size_t) 5);
        v.push_back(42);
        ASSERT_EQ(v.size(), (size_t) 6);
        ASSERT_EQ(v.back(), 42);
        ASSERT_EQ(v.front(), 3);
        v.pop_back();
        ASSERT_EQ(v.size(), (size_t) 5);
    }

    {
        oplib::Vector<int> v(10);
        ASSERT_EQ(v.size(), (size_t) 10);
        ASSERT_EQ(v.front(), 0);
        ASSERT_FALSE(v.empty());
    }
}

TEST_F(vectorTest, copy_assign_test)
{
    oplib::Vector<std::string> v(5, "good");
    oplib::Vector<std::string> vc(v);
    ASSERT_EQ(vc.size(), (size_t) 5);
    ASSERT_EQ(vc.back(), "good");
    oplib::Vector<std::string> vassign;
    vassign = v;
    ASSERT_EQ(vassign.size(), (size_t) 5);
    ASSERT_EQ(vassign.back(), "good");
}

TEST_F(vectorTest, element_operation)
{
    oplib::Vector<int> v;
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4); 
    v.push_back(5);
    int i = 0;
    for (auto iter = v.begin(); iter != v.end(); iter++, i++)
    {
        ASSERT_EQ(*iter, i);
    }
    ASSERT_EQ(v[1], 1);
    v[1] = 5;
    ASSERT_EQ(v[1], 5);
    auto iter = v.begin() + 3;
    v.erase(iter);
    ASSERT_EQ(v[3], 4);
    ASSERT_EQ(v.size(), (size_t) 5);
    v.resize(4, 1);
    ASSERT_EQ(v.size(), (size_t) 4);
    v.resize(6, 1);
    ASSERT_EQ(v.size(), (size_t) 6);
    ASSERT_EQ(v[5], 1);
    v.insert(v.begin(), 2, 10);
    ASSERT_EQ(v.size(), (size_t) 8);
    ASSERT_EQ(v[0], v[1]);
    ASSERT_EQ(v[0], 10);
    v.clear();
    ASSERT_EQ(v.size(), (size_t) 0);
}

TEST_F(vectorTest, test_vector_size)
{
    oplib::Vector<int> v;
    std::cout << "Size of oplib::Vector is " << sizeof(v) << std::endl;
}
