#include "alloc.H"
#include "testfoo.h"

using ::testing::Return;
using oplib::opallocator;

FooTest::FooTest() {
    // Have qux return true by default
    //ON_CALL(m_bar,qux()).WillByDefault(Return(true));
    // Have norf return false by default
    //ON_CALL(m_bar,norf()).WillByDefault(Return(false));
}

FooTest::~FooTest() {};

void FooTest::SetUp() {};

void FooTest::TearDown() {};

TEST_F(FooTest, alloctest) {
    opallocator<int> alloc;
    int* addr = nullptr;
    addr = alloc.allocate(3);
    ASSERT_NE(addr, nullptr);

    alloc.construct(addr, 1);
    alloc.construct(addr + 1, 2);
    alloc.construct(addr + 2, 3);
    ASSERT_EQ(*addr, 1);
    ASSERT_EQ(*(addr + 1), 2);
    ASSERT_EQ(*(addr + 2), 3);

    alloc.deallocate(addr, 3);
}

