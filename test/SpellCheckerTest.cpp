
#include "gtest/gtest.h"
#include "../SpellChecker.h"

namespace
{

using StringVec = SpellChecker::StringVec;
using StringSet = SpellChecker::StringSet;
using StringSetPair = SpellChecker::StringSetPair;

TEST(SpellCheckerTest, CreateMasks)
{
   EXPECT_EQ(StringSetPair({ { "*" }, {} }), SpellChecker::CreateMasks(""));

   EXPECT_EQ(StringSetPair({ "", "*a", "a*"}, {"*a*", "*" }), SpellChecker::CreateMasks("a"));
   EXPECT_EQ(StringSetPair({ "a",
                             "*aa", "a*a", "aa*" },
                           { "*a", "a*", "*aa*", "*a*a", "a*a*" }), SpellChecker::CreateMasks("aa"));
   EXPECT_EQ(StringSetPair({ "a", "b",
                         "*ab", "a*b", "ab*" },
      { "*b", "a*", "b*", "*a",
          "*ab*", "*a*b", "a*b*" }), SpellChecker::CreateMasks("ab"));
   EXPECT_EQ(StringSetPair({ "bc", "ac", "ab",
                         "*abc", "a*bc", "ab*c", "abc*" },
      { "*a*bc", "*ab*c",  "*abc*","a*b*c", "a*bc*",  "ab*c*", // 2 ins
        "b", // 2 dels
       "*ab",  "*ac", "*bc", "a*b",   "a*c", "ab*", "ac*", "b*c", "bc*" }),
      SpellChecker::CreateMasks("abc"));
}

TEST(SpellCheckerTest, CheckSpellingSimple)
{
   using Result = SpellChecker::SpellCheckingRes;
   SpellChecker checker;
   checker.AddWord("rain");
   checker.AddWord("zoom");

   EXPECT_EQ(Result(SpellChecker::Correction::No, {"rain"} ), checker.CheckSpelling("rain"));
   EXPECT_EQ(Result(SpellChecker::Correction::One, {"rain"} ), checker.CheckSpelling("brain"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { "rain" }), checker.CheckSpelling("brainy"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { }), checker.CheckSpelling("grainst"));
}

TEST(SpellCheckerTest, CheckSpellingSample1)
{
   using Result = SpellChecker::SpellCheckingRes;
   SpellChecker checker;
   checker.AddWords({ "rain", "spain",  "plain",  "plaint",  "pain",  "main",  "mainly",
                      "the",  "in",  "on",  "fall",  "falls",  "his",  "was" });

   EXPECT_EQ(Result(SpellChecker::Correction::Two, { "the" }), checker.CheckSpelling("hte"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { }), checker.CheckSpelling("rame"));
   EXPECT_EQ(Result(SpellChecker::Correction::No, { "in" }), checker.CheckSpelling("in"));
   EXPECT_EQ(Result(SpellChecker::Correction::No, { "pain" }), checker.CheckSpelling("pain"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { "falls" }), checker.CheckSpelling("fells"));
   EXPECT_EQ(Result(SpellChecker::Correction::One, { "main", "mainly" }), checker.CheckSpelling("mainy"));
   EXPECT_EQ(Result(SpellChecker::Correction::One, { "on" }), checker.CheckSpelling("oon"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { "the" }), checker.CheckSpelling("teh"));
   EXPECT_EQ(Result(SpellChecker::Correction::One, { "plain" }), checker.CheckSpelling("lain"));
   EXPECT_EQ(Result(SpellChecker::Correction::No, { "was" }), checker.CheckSpelling("was"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { }), checker.CheckSpelling("hints"));
   EXPECT_EQ(Result(SpellChecker::Correction::Two, { "plaint" }), checker.CheckSpelling("pliant"));
}

TEST(SpellCheckerTest, CheckSpellingLongestWord)
{
   using Result = SpellChecker::SpellCheckingRes;
   SpellChecker checker;
   const char* const longestWord = "pneumonoultramicroscopicsilicovolcanoconiosis";
   checker.AddWords({ longestWord });

   EXPECT_EQ(Result(SpellChecker::Correction::Two, { longestWord }), checker.CheckSpelling("pneumonoltramicroscopicsilicovolcanooniosis"));
   EXPECT_EQ(Result(SpellChecker::Correction::One, { longestWord }), checker.CheckSpelling("pneumonoultramicroscopicsilicovolcanoconiosiss"));
}

}