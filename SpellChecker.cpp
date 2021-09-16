#include "SpellChecker.h"
#include <cassert>
#include <thread>

namespace
{

using StringSet = SpellChecker::StringSet;

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

SpellChecker::StringSetPair SpellChecker::CreateMasks(const std::string& word)
{
   StringSet oneCorrectionMask;
   StringSet twoCorrectionsMask;
   createDeletionMasks(word, oneCorrectionMask, twoCorrectionsMask);
   createInsertionMasks(word, oneCorrectionMask, twoCorrectionsMask);
   createInsertionAndDeletionMasks(word, twoCorrectionsMask);

   return { oneCorrectionMask, twoCorrectionsMask };
}

SpellChecker::SpellCheckingRes SpellChecker::CheckSpelling(const std::string& word) const
{
   const StringVec exactMatching = m_trie.FindAll(word);
   if (!exactMatching.empty())
   {
      assert(exactMatching[0] == word);
      return { Correction::No, { word } };
   }

   const auto& [oneCorrectionMask, twoCorrectionsMask] = CreateMasks(word);

   StringSet candidates;
   for (const auto& mask : oneCorrectionMask)
   {
      StringVec oneCorrections = m_trie.FindAll(mask);
      candidates.insert(oneCorrections.begin(), oneCorrections.end());
   }

   if(!candidates.empty())
   {
      return { Correction::One, candidates };
   }

   for (const auto& mask : twoCorrectionsMask)
   {
      StringVec twoCorrections = m_trie.FindAll(mask);
      candidates.insert(twoCorrections.begin(), twoCorrections.end());
   }

   return { Correction::Two, candidates};
}

#if 0
SpellChecker::StringSet SpellChecker::checkSpellingAsync(const StringSet& masks) const
{
   std::promise<StringSet> candidatePromise;
   std::futures<StringSet> candidateFuture = candidatePromise.get_future();
   std::vector<std::thread> threads;
   for (auto first = masks.begin(); first != masks.end();)
   {
      auto end = first;
      std::advance(end, masks.size() / 4);

      StringSet myMasks = StringSet(first, end);
      first = end;
      threads.emplace_back(std::thread([myMasks = std::move(myMasks), &m_trie]()
      {
         StringSet candidates;
         for (const auto& mask : myMasks)
         {
            StringVec corrections = m_trie.FindAll(mask);
            candidates.insert(corrections.begin(), corrections.end());
         }
         //StringVec oneCorrections = m_trie.FindAll(mask);
        // candidates.insert(oneCorrections.begin(), oneCorrections.end());
      }));

      
   }
}

#endif