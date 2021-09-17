#pragma once

#include "Trie.h"

#include <string>
#include <vector>
#include <set>

/// <summary>
/// Class to check a word spelling
/// 1. Build a dictionary with <code>AddWord</code> calls
/// 2. Check spelling with <code>CheckSpelling</code>
/// </summary>
class WordSpellChecker
{
public:
   using StringVec = std::vector<std::string>;
   using StringSet = std::set<std::string>;
   using StringSetPair = std::pair<StringSet, StringSet>;

   /// <summary>
   /// Adds a word to the dictionary
   /// </summary>
   /// <param name="word"></param>
   /// 
   void AddWord(const std::string& word);
   template<typename Itr>
   /// <summary>
   /// Adds words from a collection
   /// </summary>
   /// <typeparam name="Itr">ForwardIterator type</typeparam>
   /// <param name="first">iterator pointing at the beginning of the collection</param>
   /// <param name="last">iterator pointing at the element after the end of the collection</param>
   void AddWords(Itr first, Itr last);

   /// <summary>
   /// Adds words from an initialization list
   /// </summary>
   /// <param name="list">list of words</param>
   void AddWords(std::initializer_list<std::string> list);

   enum class Correction
   {
      No,  ///< No correction, correct word:  word - word
      One, ///< One correction, deletion or insertion: wor, worda - word
      Two  ///< Two corrections, wors, wr, aworda - word
   };

   using WordAndCorrection = std::pair<std::string, Correction>;
   using SpellCheckingRes = std::pair<Correction, StringSet>;

   /// <summary>
   /// Creates a collection of masks to match against: insertion is designated by '?'.
   /// Two deletion or insertion allowed, but 2 subsequent deletions or insertions are not.
   /// word ->
   ///  OK: word?, wor?, wr; 
   ///  Wrong: wo??, wo
   /// Note: symbol substitution (worz -> word) is a deletion + insertion operation;
   /// </summary>
   /// <param name="word">word to create masks for</param>
   /// <returns>Pair of 2 sets: one correction allowed, two correction allowed</returns>
   static StringSetPair CreateMasks(const std::string& word);

   /// <summary>
   /// Checks a word spelling against the built dictionary and suggest corrections
   /// </summary>
   /// <param name="word">word to check</param>
   /// <returns>0-2 correction to apply + corrected word from the dictionary</returns>
   SpellCheckingRes CheckSpelling(const std::string& word) const;

private:

   StringSet checkMasks(const StringSet& masks) const;
   StringSet checkSpellingAsync(const StringSet& masks) const;

   trie::Trie m_trie;
};

inline void WordSpellChecker::AddWord(const std::string& word)
{
   m_trie.Add(word);
}

template<typename Itr>
inline void WordSpellChecker::AddWords(Itr first, Itr last)
{
   for (; first != last; ++first)
   {
      AddWord(*first);
   }
}

inline void WordSpellChecker::AddWords(std::initializer_list<std::string> list)
{
   AddWords(list.begin(), list.end());
}

