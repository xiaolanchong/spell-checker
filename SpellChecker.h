#pragma once

#include "Trie.h"

#include <string>
#include <vector>
#include <set>

class SpellChecker
{
public:
   using StringVec = std::vector<std::string>;
   using StringSet = std::set<std::string>;
   using StringSetPair = std::pair<StringSet, StringSet>;

   void AddWord(const std::string& word);

   template<typename Itr>
   void AddWords(Itr first, Itr last)
   {
      for (;first != last; ++first)
      {
         AddWord(*first);
      }
   }

   void AddWords(std::initializer_list<std::string> list)
   {
      AddWords(list.begin(), list.end());
   }

   enum class Correction
   {
      No,
      One,
      Two
   };

   using WordAndCorrection = std::pair<std::string, Correction>;
   using SpellCheckingRes = std::pair<Correction, StringSet>;

   /// <summary>
   /// For 1 and 2 corrections
   /// </summary>
   /// <param name="word">word to create masks for</param>
   /// <returns>Pair of 2 sets</returns>
   static StringSetPair CreateMasks(const std::string& word);

   SpellCheckingRes CheckSpelling(const std::string& word) const;

private:

   StringSet checkSpellingAsync(const StringSet& masks) const;

   trie::Trie m_trie;
};

inline void SpellChecker::AddWord(const std::string& word)
{
   m_trie.Add(word);
}

