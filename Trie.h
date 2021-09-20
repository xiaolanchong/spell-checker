#pragma once

#include <string>
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

   /// <summary>
   /// Symbol to designate any letter in a word
   /// </summary>
   static const char sc_anyLetter = '?';

   explicit TrieNode(char letter)
      : m_letter(letter) 
      , m_canBeTerminal(false)
   {
      m_children.reserve(size_t('z' - 'a') + 1);
   }

   TrieNode(TrieNode&&) = default;
   TrieNode& operator =(TrieNode&&) = default;

   /// <summary>
   /// Adds a word or its remaining part
   /// </summary>
   /// <param name="suffix"></param>
   void AddSuffix(const std::string& suffix);

   /// <summary>
   /// Finds all words matching the mask
   /// </summary>
   /// <param name="matchedSoFar"></param>
   /// <param name="mask">a-z or ? (any of a-z)</param>
   /// <param name="onFound">Function to call if a word found</param>
   void FindAll(const std::string& matchedSoFar, const std::string& mask, const FnFound& onFound) const;

   char GetLetter() const { return m_letter; }
private:
   TrieNode(const TrieNode&) = delete;
   TrieNode& operator =(const TrieNode&) = delete;

   using TrieNodeVec = std::vector<TrieNode>;
   using FoundAndWhere = std::pair<bool, TrieNodeVec::iterator>;
   
   /// <summary>
   /// Finds a child with the letter or place to insert into 
   /// </summary>
   /// <param name="letter"></param>
   /// <returns>pair: found or not, if found the iterator to, if not where to insert not to break the order</returns>
   FoundAndWhere findChildOrWhereToInsert(char letter);

   /// <summary>
   /// Finds the child with the given letter
   /// </summary>
   /// <param name="letter">Any of a-z</param>
   /// <returns>A valid iterator, end() if not found</returns>
   TrieNodeVec::const_iterator findChild(char letter) const;

   /// <summary>
   /// Letter this node represents
   /// </summary>
   char m_letter;

   /// <summary>
   /// Is the letter the last one of a word in the dictionary
   /// </summary>
   bool m_canBeTerminal;

   std::vector<TrieNode> m_children;
};

}

/// <summary>
/// A tree with letter nodes, each descent represents a word:
///           root
///            w a
///           /   \
///         a      r
///        /\    / | \
///       r  s  c  k  m*
///                    \
///                     y
/// Dictionary:  war, was, arc, ark, arm, army
/// Children inside a node is sorted for O(logN) search
/// * designates a mark of the end of a word
/// See https://en.wikipedia.org/wiki/Trie
/// </summary>
class Trie
{
public:

   using StringVec = internal::TrieNode::StringVec;

   static const char sc_anyLetter = internal::TrieNode::sc_anyLetter;

   Trie();

   /// <summary>
   /// Adds a word to the tree, ignored if already exists
   /// </summary>
   /// <param name="word">Word to add</param>
   void Add(const std::string& word);

   /// <summary>
   /// Finds a word by mask, e.g. was -> was, wa? -> war, was (see trie in the header)
   /// </summary>
   /// <param name="word">string of a-z and ? symbols</param>
   /// <returns>Collection of matching words</returns>
   StringVec FindAll(const std::string& mask) const;

private:
   Trie(const Trie&) = delete;
   Trie& operator =(const Trie&) = delete;

   /// <summary>
   /// pseudo letter to create a root
   /// </summary>
   static const char sc_rootLetter = char(-1);

   internal::TrieNode m_root;
};

}