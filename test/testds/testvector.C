#include "alloc.H"
#include "testvector.H"

vectorTest::vectorTest() {}

vectorTest::~vectorTest() {}

void vectorTest::SetUp() {}

void vectorTest::TearDown() {}

TEST_F(vectorTest, basic_operation_test) 
{
    oplib::Vector<int> v(5, 3);
    ASSERT_EQ(v.size(), (size_t) 5);
}

