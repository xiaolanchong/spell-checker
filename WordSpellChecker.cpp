#include "WordSpellChecker.h"
#include <cassert>
#include <thread>
#include <future>

namespace
{

using StringSet = WordSpellChecker::StringSet;


template<class It>
constexpr void advanceWithEndChecking(It& it, size_t n, It end)
{
   for (size_t i = 0; i < n && it != end; ++i, ++it)
   {
   }
}

void createDeletionMasks(const std::string& word, StringSet& oneCorrectionMask, StringSet& twoCorrectionsMask)
{
   // deletion
   for (size_t delPos = 0; delPos < word.size(); ++delPos)
   {
      auto afterDeletion = word;
      afterDeletion.erase(delPos, 1);
      oneCorrectionMask.insert(afterDeletion);

      // deletion + deletion
      for (size_t delAgainPos = 0; delAgainPos < afterDeletion.size(); ++delAgainPos)
      {
         if (delAgainPos == delPos || delAgainPos + 1 == delPos)
         {
            continue; // 2 succeeding deletion
         }
         auto afterAfterDeletion = afterDeletion;
         afterAfterDeletion.erase(delAgainPos, 1);
         twoCorrectionsMask.emplace(std::move(afterAfterDeletion));
      }
   }
}

void createInsertionMasks(const std::string& word, StringSet& oneCorrectionMask, StringSet& twoCorrectionsMask)
{
   // insertion
   for (size_t insPos = 0; insPos <= word.size(); ++insPos)
   {
      auto afterInsertion = word;
      afterInsertion.insert(afterInsertion.begin() + insPos, 1, trie::Trie::sc_anyLetter);
      oneCorrectionMask.insert(afterInsertion);

      // insertion + insertion
      for (size_t insAgainPos = 0; insAgainPos <= afterInsertion.size(); ++insAgainPos)
      {
         if (insAgainPos == insPos || insAgainPos == insPos + 1)
         {
            continue; // 2 succeeding insertion
         }
         auto afterAfterInsertion = afterInsertion;
         afterAfterInsertion.insert(afterAfterInsertion.begin() + insAgainPos, 1, trie::Trie::sc_anyLetter);
         twoCorrectionsMask.emplace(std::move(afterAfterInsertion));
      }
   }
}

void createInsertionAndDeletionMasks(const std::string& word, StringSet& twoCorrectionsMask)
{
   for (size_t insPos = 0; insPos <= word.size(); ++insPos)
   {
      auto afterInsertion = word;
      afterInsertion.insert(afterInsertion.begin() + insPos, 1, trie::Trie::sc_anyLetter);

      for (size_t delPos = 0; delPos < afterInsertion.size(); ++delPos)
      {
         if (delPos == insPos)
         {
            continue; // insertion + deletion at the same position -> original
         }
         auto afterDeletion = afterInsertion;
         afterDeletion.erase(delPos, 1);
         twoCorrectionsMask.emplace(std::move(afterDeletion));
      }
   }
}

}

WordSpellChecker::StringSetPair WordSpellChecker::CreateMasks(const std::string& word)
{
   StringSet oneCorrectionMask;
   StringSet twoCorrectionsMask;
   createDeletionMasks(word, oneCorrectionMask, twoCorrectionsMask);
   createInsertionMasks(word, oneCorrectionMask, twoCorrectionsMask);
   createInsertionAndDeletionMasks(word, twoCorrectionsMask);

   return { oneCorrectionMask, twoCorrectionsMask };
}

WordSpellChecker::SpellCheckingRes WordSpellChecker::CheckSpelling(const std::string& word) const
{
   const StringVec exactMatching = m_trie.FindAll(word);
   if (!exactMatching.empty())
   {
      assert(exactMatching[0] == word);
      return { Correction::No, { word } };
   }

   const auto& [oneCorrectionMask, twoCorrectionsMask] = CreateMasks(word);
   auto candidates = checkSpellingAsync(oneCorrectionMask);
   if (!candidates.empty())
   {
      return { Correction::One, candidates };
   }

   candidates = checkSpellingAsync(twoCorrectionsMask);
   return { Correction::Two, candidates };
}

WordSpellChecker::StringSet WordSpellChecker::checkMasks(const StringSet& masks) const
{
   StringSet candidates;
   for (const auto& mask : masks)
   {
      StringVec corrections = m_trie.FindAll(mask);
      candidates.insert(corrections.begin(), corrections.end());
   }
   return candidates;
}

WordSpellChecker::StringSet WordSpellChecker::checkSpellingAsync(const StringSet& masks) const
{
   const size_t maskNumberInChunk = 10;

   if (masks.size() <= maskNumberInChunk)
   {
      return checkMasks(masks);
   }

   std::vector<std::future<StringSet>> tasks;
   for (auto start = masks.begin(); start != masks.end();)
   {
      auto end = start;
      advanceWithEndChecking(end, maskNumberInChunk, masks.end());

      StringSet myMasks(start, end);
      start = end;

      auto fut = std::async(std::launch::async, [myMasks=std::move(myMasks), this]()
      {
         return checkMasks(myMasks);
      });
      tasks.emplace_back(std::move(fut));
   }

   StringSet result;
   for (auto& task : tasks)
   {
      StringSet taskResult = task.get();
      result.insert(taskResult.begin(), taskResult.end());
   }
   return result;
}
