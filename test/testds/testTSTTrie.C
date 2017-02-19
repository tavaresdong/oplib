#include "gtest/gtest.h"
#include <ds/TSTTrie.h>
#include <string>
#include <algorithm>
#include <iostream>

// The fixture for testing class Foo.
class TSTTrieTest : public ::testing::Test 
{
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

  oplib::TSTTrie<int> getTrie()
  {
    oplib::TSTTrie<int> trie;
    trie.put("good", 3);
    trie.put("goo", 2);
    trie.put("goat", 1);
    trie.put("government", 1);
    trie.put("gas", 1);
    trie.put("del", 1);
    trie.put("xx", 3);
    return trie;
  }

  oplib::TSTTrie<bool> getTrieComplex()
  {
    oplib::TSTTrie<bool> trie;
    trie.put("app", true);
    trie.put("apple", true);
    trie.put("beer", true);
    trie.put("add", true);
    trie.put("jam", true);
    trie.put("rental", true);

    return trie;
  }
};

TSTTrieTest::TSTTrieTest() {}

TSTTrieTest::~TSTTrieTest() {}

void TSTTrieTest::SetUp() {}

void TSTTrieTest::TearDown() {}


TEST_F(TSTTrieTest, TriePutGetContainsTest) 
{
  oplib::TSTTrie<int> trie { getTrie() };
 
  EXPECT_TRUE(trie.contains("good"));
  EXPECT_EQ(trie.get("good").first, 3);
  EXPECT_TRUE(trie.contains("xx"));
  EXPECT_EQ(trie.get("xx").first, 3);
 
  EXPECT_FALSE(trie.contains("delete"));
  EXPECT_FALSE(trie.contains("go"));
}

TEST_F(TSTTrieTest, TrieCopyAndSearchTest)
{
  oplib::TSTTrie<bool> trie { getTrieComplex() };

  // Copy ctor
  oplib::TSTTrie<bool> copiedTrie(trie);
  oplib::TSTTrie<bool> trie2;
  trie2.put("jan", true);

  // Assign
  trie2 = trie;

  EXPECT_FALSE(copiedTrie.contains("jan"));
  EXPECT_TRUE(copiedTrie.contains("jam"));
  EXPECT_FALSE(trie2.contains("jan"));
  EXPECT_TRUE(trie2.contains("jam"));
}


TEST_F(TSTTrieTest, TrieKeyWithPrefixTest)
{
  auto trie { getTrie() };

  EXPECT_EQ(trie.keysWithPrefix("go").size(), (size_t) 4);
  EXPECT_EQ(trie.keysWithPrefix("g").size(), (size_t) 5);
  EXPECT_EQ(trie.keysWithPrefix("xx").size(), (size_t) 1);
  EXPECT_EQ(trie.keysWithPrefix("goody").size(), (size_t) 0);
  EXPECT_EQ(trie.keys().size(), (size_t) 7);

  auto ret = trie.keysWithPrefix("go");
  EXPECT_TRUE(std::find(ret.begin(), ret.end(), "good") != ret.end());
  EXPECT_TRUE(std::find(ret.begin(), ret.end(), "goo") != ret.end());
  EXPECT_TRUE(std::find(ret.begin(), ret.end(), "gas") == ret.end());
}

TEST_F(TSTTrieTest, TrieLongestPrefixTest)
{
  oplib::TSTTrie<int> trie { getTrie() };
  
  EXPECT_EQ(trie.longestPrefixOf("goody"), std::string("good"));
  EXPECT_EQ(trie.longestPrefixOf("goose"), std::string("goo"));
  EXPECT_EQ(trie.longestPrefixOf("delete"), std::string("del"));
  EXPECT_EQ(trie.longestPrefixOf("xt"), std::string(""));
}
