#pragma once

#include "WordSpellChecker.h"
#include <string>
#include <utility>

class TextSpellChecker
{
public:
   void AddWordToDictionary(const std::string& word);
   void AddWordToDictionary(std::initializer_list<std::string> list);

   std::string CheckText(const std::string& text) const;
private:

   enum class TokenType
   {
      Word,
      Other
   };

   using Token = std::pair<TokenType, std::string>;
   using TokenVec = std::vector<Token>;

   static TokenVec tokenize(const std::string& text);

   WordSpellChecker m_wordChecker;
};

inline void TextSpellChecker::AddWordToDictionary(const std::string& word)
{
   m_wordChecker.AddWord(word);
}

inline void TextSpellChecker::AddWordToDictionary(std::initializer_list<std::string> list)
{
   m_wordChecker.AddWords(std::move(list));
}
