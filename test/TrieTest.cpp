
#include "gtest/gtest.h"
#include "../Trie.h"

namespace
{

using StringVec = trie::Trie::StringVec;

TEST(TrieTest, AddDuplicates)
{
   trie::Trie trie;
   trie.Add("aa");
   trie.Add("aa");
}

TEST(TrieTest, FindAll)
{
   trie::Trie trie;
   trie.Add("sample");

   EXPECT_EQ(StringVec{ }, trie.FindAll(""));
   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("sample"));
   EXPECT_EQ(StringVec{ }, trie.FindAll("samp"));
   EXPECT_EQ(StringVec{ }, trie.FindAll("sampl"));
   EXPECT_EQ(StringVec{ }, trie.FindAll("samples"));
}

TEST(TrieTest, FindAllWithStar)
{
   trie::Trie trie;
   trie.Add("sample");

   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("?ample"));
   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("s?mple"));
   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("sam?le"));
   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("samp?e"));
   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("sampl?"));

   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("s?mpl?"));
   EXPECT_EQ(StringVec{ "sample" }, trie.FindAll("sa?pl?"));
   EXPECT_EQ(StringVec{ }, trie.FindAll("sa?zl?"));
   EXPECT_EQ(StringVec{ }, trie.FindAll("zampl?"));

   trie.Add("sanple");

   EXPECT_EQ(StringVec({ "sample", "sanple" }), trie.FindAll("sa?ple"));
}

}