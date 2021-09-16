#pragma once

#include <string>
#include <ostream>
#include <vector>
#include <functional>

namespace trie
{

namespace internal
{

class TrieNode
{
public:
   
   using StringVec = std::vector<std::string>;
   using FnFound = std::function<void(const std::string&)>;

   static const char sc_anyLetter = '*';

   explicit TrieNode(char letter)
      : m_letter(letter) 
      , m_canBeTerminal(false)
   {
      m_children.reserve(size_t('z' - 'a') + 1);
   }

   TrieNode(TrieNode&&) = default;
   TrieNode& operator =(TrieNode&&) = default;

   void AddSuffix(const std::string& suffix);
   void FindAll(const std::string& matchedSoFar, const std::string& word, const FnFound& onFound) const;
   void Print(std::ostream& os) const;

   char GetLetter() const { return m_letter; }
private:
   TrieNode(const TrieNode&) = delete;
   TrieNode& operator =(const TrieNode&) = delete;

   using TrieNodeVec = std::vector<TrieNode>;
   using FoundAndWhere = std::pair<bool, TrieNodeVec::iterator>;
   
   FoundAndWhere findChildOrWhereToInsert(char letter);
   TrieNodeVec::const_iterator findChild(char letter) const;

   char m_letter;
   bool m_canBeTerminal;
   std::vector<TrieNode> m_children;
};

}

class Trie
{
public:

   using StringVec = internal::TrieNode::StringVec;

   static const char sc_anyLetter = internal::TrieNode::sc_anyLetter;

   Trie();
   void Add(const std::string& word);
   StringVec FindAll(const std::string& word) const;
   
   void Print(std::ostream& os) const;
private:
   Trie(const Trie&) = delete;
   Trie& operator =(const Trie&) = delete;

   static const char sc_rootLetter = char(-1);

   internal::TrieNode m_root;
};

}