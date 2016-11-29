#include "alloc.H"
#include "gtest/gtest.h"
#include "linkedlist.H"
#include <string>

// The fixture for testing class Foo.
class linkedlistTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    linkedlistTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~linkedlistTest();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();
};

linkedlistTest::linkedlistTest() {}

linkedlistTest::~linkedlistTest() {}

void linkedlistTest::SetUp() {}

void linkedlistTest::TearDown() {}

TEST_F(linkedlistTest, basic_operation)
{
    oplib::Linkedlist<int> v;
    ASSERT_TRUE(v.empty());
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4); 
    v.push_back(5);

    ASSERT_FALSE(v.empty());
    ASSERT_EQ(v.size(), (size_t) 6);

    int i = 0;
    for (auto iter = v.begin(); iter != v.end(); ++iter, ++i)
    {
        ASSERT_EQ(*iter, i);
    }

    ASSERT_EQ(v.front(), 0);
    ASSERT_EQ(v.back(), 5);

    v.push_front(-1);
    ASSERT_EQ(v.size(), (size_t) 7);
    ASSERT_EQ(v.front(), -1);
}

TEST_F(linkedlistTest, element_manipulation)
{
    oplib::Linkedlist<int> v;
    ASSERT_TRUE(v.empty());
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    
    auto iter = v.begin();
    ++iter;
    iter = v.erase(iter);
    ASSERT_EQ(*iter, 2);
    --iter;
    ASSERT_EQ(*iter, 0);

    v.pop_back();
    ASSERT_EQ(v.back(), 2);
    
    v.pop_front();
    ASSERT_EQ(v.front(), 2);

    v.push_front(3);
    v.push_back(3);
    ASSERT_EQ(v.size(), (size_t) 3);

    v.clear();
    ASSERT_TRUE(v.empty());

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(2);
    v.remove(2);
    ASSERT_EQ(v.size(), (size_t) 2);
    
    v.push_back(2);
    v.push_back(3);
    v.remove_if([](int a){ return a == 3; });
    ASSERT_EQ(v.size(), (size_t) 2);
    
    v.push_front(1);
    v.push_back(1);
    v.push_back(1);
    v.push_back(1);
    v.unique();
    ASSERT_EQ(v.size(), (size_t) 3);
    ASSERT_EQ(v.front(), 1);
    ASSERT_EQ(v.back(), 1);
}

