#include "Trie.h"

#include <algorithm>

namespace trie
{

Trie::Trie()
   : m_root(sc_rootLetter)
{
}

void Trie::Add(const std::string& word)
{
   m_root.AddSuffix(word);
}

void Trie::Print(std::ostream& os) const
{
   m_root.Print(os);
}

Trie::StringVec Trie::FindAll(const std::string& word) const
{
   StringVec result;
   m_root.FindAll({}, word,
      [&result](const std::string& foundWord)
   {
      result.push_back(foundWord);
   });
   return result;
}

namespace internal
{

bool operator < (char left, const TrieNode& right)
{
   return left < right.GetLetter();
}

bool operator < (const TrieNode& left, char right)
{
   return left.GetLetter() < right;
}

void TrieNode::AddSuffix(const std::string& suffix)
{
   if (suffix.empty())
   {
      m_canBeTerminal = true;
      return;
   }

   const auto prefix = suffix[0];
   const auto newSuffix = suffix.substr(1);
   auto [found, itEqualOrGreater] = findChildOrWhereToInsert(prefix);
   if (found)
   {
      itEqualOrGreater->AddSuffix(newSuffix);
   }
   else
   {
      TrieNode newNode(prefix);
      newNode.AddSuffix(newSuffix);
      m_children.emplace(itEqualOrGreater, std::move(newNode));
   }
}

TrieNode::TrieNodeVec::const_iterator TrieNode::findChild(char letter) const
{
   const auto itEqualOrGreater = std::lower_bound(m_children.cbegin(), m_children.cend(), letter);
   if (itEqualOrGreater != m_children.end() &&
      itEqualOrGreater->GetLetter() == letter)
   {
      return itEqualOrGreater;
   }
   return m_children.cend();
}

TrieNode::FoundAndWhere TrieNode::findChildOrWhereToInsert(char letter)
{
   const auto itEqualOrGreater = std::lower_bound(m_children.begin(), m_children.end(), letter);
   if (itEqualOrGreater != m_children.end() &&
      itEqualOrGreater->GetLetter() == letter)
   {
      return std::make_pair(true, itEqualOrGreater);
   }
   return std::make_pair(false, itEqualOrGreater);
}

void TrieNode::FindAll(const std::string& matchedSoFar, const std::string& word, const FnFound& onFound) const
{
   if (word.empty())
   {
      if (m_canBeTerminal)
      {
         onFound(matchedSoFar);
      }
      return;
   }

   const auto letter = word[0];
   const auto rest = word.substr(1);

   if (letter == sc_anyLetter)
   {
      for (const auto& child: m_children)
      {
         const auto newMatchedSoFar = matchedSoFar + child.GetLetter();
         child.FindAll(newMatchedSoFar, rest, onFound);
      }
   }
   else
   {
      const auto itWhere = findChild(letter);
      if (itWhere != m_children.cend())
      {
         const auto newMatchedSoFar = matchedSoFar + itWhere->GetLetter();
         itWhere->FindAll(newMatchedSoFar, rest, onFound);
      }
   }
}

void TrieNode::Print(std::ostream& /*os*/) const
{
}



}

}