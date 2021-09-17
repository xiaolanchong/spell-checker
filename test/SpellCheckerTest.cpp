
#include "../WordSpellChecker.h"
#include "../TextSpellChecker.h"
#include "gtest/gtest.h"
#include <fstream>

namespace
{

using StringVec = WordSpellChecker::StringVec;
using StringSet = WordSpellChecker::StringSet;
using StringSetPair = WordSpellChecker::StringSetPair;

TEST(SpellCheckerTest, CreateMasks)
{
   EXPECT_EQ(StringSetPair({ { "?" }, {} }), WordSpellChecker::CreateMasks(""));

   EXPECT_EQ(StringSetPair({ "", "?a", "a?"}, {"?a?", "?" }), WordSpellChecker::CreateMasks("a"));
   EXPECT_EQ(StringSetPair({ "a",
                             "?aa", "a?a", "aa?" },
                           { "?a", "a?", "?aa?", "?a?a", "a?a?" }), WordSpellChecker::CreateMasks("aa"));
   EXPECT_EQ(StringSetPair({ "a", "b",
                         "?ab", "a?b", "ab?" },
      { "?b", "a?", "b?", "?a",
          "?ab?", "?a?b", "a?b?" }), WordSpellChecker::CreateMasks("ab"));
   EXPECT_EQ(StringSetPair({ "bc", "ac", "ab",
                         "?abc", "a?bc", "ab?c", "abc?" },
      { "?a?bc", "?ab?c",  "?abc?","a?b?c", "a?bc?",  "ab?c?", // 2 ins
        "b", // 2 dels
       "?ab",  "?ac", "?bc", "a?b",   "a?c", "ab?", "ac?", "b?c", "bc?" }),
      WordSpellChecker::CreateMasks("abc"));
}

TEST(SpellCheckerTest, CheckSpellingSimple)
{
   using Result = WordSpellChecker::SpellCheckingRes;
   WordSpellChecker checker;
   checker.AddWord("rain");
   checker.AddWord("zoom");

   EXPECT_EQ(Result(WordSpellChecker::Correction::No, {"rain"} ), checker.CheckSpelling("rain"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::One, {"rain"} ), checker.CheckSpelling("brain"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { "rain" }), checker.CheckSpelling("brainy"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { }), checker.CheckSpelling("grainst"));
}

TEST(SpellCheckerTest, CheckSpellingSample1)
{
   using Result = WordSpellChecker::SpellCheckingRes;
   WordSpellChecker checker;
   checker.AddWords({ "rain", "spain",  "plain",  "plaint",  "pain",  "main",  "mainly",
                      "the",  "in",  "on",  "fall",  "falls",  "his",  "was" });

   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { "the" }), checker.CheckSpelling("hte"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { }), checker.CheckSpelling("rame"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::No, { "in" }), checker.CheckSpelling("in"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::No, { "pain" }), checker.CheckSpelling("pain"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { "falls" }), checker.CheckSpelling("fells"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::One, { "main", "mainly" }), checker.CheckSpelling("mainy"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::One, { "on" }), checker.CheckSpelling("oon"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { "the" }), checker.CheckSpelling("teh"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::One, { "plain" }), checker.CheckSpelling("lain"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::No, { "was" }), checker.CheckSpelling("was"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { }), checker.CheckSpelling("hints"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { "plaint" }), checker.CheckSpelling("pliant"));
}

using Result = WordSpellChecker::SpellCheckingRes;

TEST(SpellCheckerTest, CheckSpellingLongestWord)
{
   WordSpellChecker checker;
   const char longestWord[] = "pneumonoultramicroscopicsilicovolcanoconiosis";
   checker.AddWords({ longestWord });

   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { longestWord }), checker.CheckSpelling("pneumonoltramicroscopicsilicovolcanooniosis"));
   EXPECT_EQ(Result(WordSpellChecker::Correction::One, { longestWord }), checker.CheckSpelling("pneumonoultramicroscopicsilicovolcanoconiosiss"));
}

TEST(SpellCheckerTest, 50kWords)
{
   std::ifstream vocFile;
   vocFile.open("../data/50k_most_freq_words.txt");
   ASSERT_TRUE(vocFile.is_open());
   StringVec vocabulary;
   std::copy(std::istream_iterator<std::string>(vocFile),
      std::istream_iterator<std::string>(),
      std::back_inserter(vocabulary));

   WordSpellChecker checker;
   checker.AddWords(vocabulary.begin(), vocabulary.end());
   EXPECT_EQ(Result(WordSpellChecker::Correction::Two, { "vocabulary" }), checker.CheckSpelling("vocubulary"));
}

TEST(SpellCheckerTest, SimpleText)
{
   TextSpellChecker checker;
   checker.AddWordToDictionary({ "know", "how", "to", "parse" });
   auto res = checker.CheckText("i now how\nto barse");
   EXPECT_EQ("{i?} know how\nto parse", res);
}

TEST(SpellCheckerTest, AssignmentSampleText)
{
   TextSpellChecker checker;
   checker.AddWordToDictionary({ "rain", "spain",  "plain",  "plaint",  "pain",  "main",  "mainly",
                      "the",  "in",  "on",  "fall",  "falls",  "his",  "was" });
   auto res = checker.CheckText("hte rame in pain fells\nmainy oon teh lain\nwas hints pliant");
   EXPECT_EQ("the {rame?} in pain falls\n{main mainly} on the plain\nwas {hints?} plaint", res);
}

}