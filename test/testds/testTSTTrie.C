#include "alloc.H"
#include "gtest/gtest.h"
#include <ds/TSTTrie.h>
#include <string>
#include <iostream>

// The fixture for testing class Foo.
class TSTTrieTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    TSTTrieTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~TSTTrieTest();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();
};

TSTTrieTest::TSTTrieTest() {}

TSTTrieTest::~TSTTrieTest() {}

void TSTTrieTest::SetUp() {}

void TSTTrieTest::TearDown() {}

TEST_F(TSTTrieTest, TriePutTest) 
{
  oplib::TSTTrie<int> trie;
  trie.put("good", 3);
  trie.put("goo", 2);
  trie.put("gas", 1);
  trie.put("del", 1);
  trie.put("xx", 3);
 
  ASSERT_TRUE(trie.contains("good"));
  ASSERT_TRUE(trie.contains("xx"));
  ASSERT_FALSE(trie.contains("delete"));
  ASSERT_FALSE(trie.contains("go"));
}

