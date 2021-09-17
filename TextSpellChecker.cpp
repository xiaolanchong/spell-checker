#include "TextSpellChecker.h"
#include <algorithm>
#include <cassert>

namespace
{

bool isLetter(char symbol)
{
   return ('a' <= symbol && symbol <= 'z') ||
          ('A' <= symbol && symbol <= 'Z');
}

std::string join(const WordSpellChecker::StringSet& words)
{
   if (words.empty())
      return {};

   if (words.size() == 1)
      return *words.cbegin();

   std::string res = "{";
   for (const auto& word: words)
   {
      res += word;
      res += ' ';
   }
   res.back() = '}';

   return res;
}

std::string outputCorrection(const std::string& word, const WordSpellChecker::SpellCheckingRes& corrections)
{
   const auto& [correctionType, suggestions] = corrections;
   switch (correctionType)
   {
   case WordSpellChecker::Correction::No:
      assert(suggestions.size() == 1);
      return *suggestions.cbegin();

   case WordSpellChecker::Correction::One:
      return join(suggestions);

   case WordSpellChecker::Correction::Two:
      if (suggestions.empty())
      {
         std::string out("{");
         out += word;
         out += "?}";
         return out;
      }
      return join(suggestions);

   default:
      assert(!"Unknown correction type");
      return {};
   }
}

}

TextSpellChecker::TokenVec TextSpellChecker::tokenize(const std::string& text)
{
   enum class State
   {
      Start,
      InWord,
      InOther
   };

   State state = State::Start;
   std::string tokenText;
   TokenVec tokens;

   for (auto pos = text.cbegin(); pos != text.cend(); ++pos)
   {
      const char curSymbol = *pos;
      switch (state)
      {
      case State::Start:
      {
         state = isLetter(curSymbol) ? State::InWord : State::InOther;
         tokenText += curSymbol;
         break;
      }
      case State::InWord:
      {
         if (isLetter(curSymbol))
         {
            tokenText += curSymbol;
         }
         else
         {
            state = State::InOther;
            tokens.emplace_back(Token{TokenType::Word, std::move(tokenText)});
            tokenText = curSymbol;
         }
         break;
      }
      case State::InOther:
      {
         if (isLetter(curSymbol))
         {
            state = State::InWord;
            tokens.emplace_back(Token{ TokenType::Other, std::move(tokenText) });
            tokenText = curSymbol;
         }
         else
         {
            tokenText += curSymbol;
         }
         break;
      }
      default:
         assert(!"Unknown tokenizer state");
         break;
      }
   }

   switch (state)
   {
   case State::Start:
   {
      break;
   }
   case State::InWord:
   {
      tokens.emplace_back(Token{ TokenType::Word, std::move(tokenText) });
      break;
   }
   case State::InOther:
   {
      tokens.emplace_back(Token{ TokenType::Other, std::move(tokenText) });
      break;
   }
   }
   return tokens;
}

std::string TextSpellChecker::CheckText(const std::string& text) const
{
   std::string output;
   const auto tokens = tokenize(text);
   for (const auto& [tokenType, tokenText]: tokens)
   {
      switch (tokenType)
      {
      case TokenType::Word:
      {
         std::string lowerText = tokenText;
         std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

         const auto res = m_wordChecker.CheckSpelling(tokenText);
         const auto toOutput = outputCorrection(tokenText, res);
         output += toOutput;
         break;
      }
        
      case TokenType::Other:
         output += tokenText;
         break;

      default:
         assert(!"Unknown token type");
         break;
      }
   }
   return output;
}
