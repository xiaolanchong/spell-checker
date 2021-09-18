#include "TextSpellChecker.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>

namespace
{

const size_t gc_maxLinesInFile = 2000;
const size_t gc_maxWordLength = 50;

/// <summary>
/// Delimiter between dictionary and text and between text and the stop
/// </summary>
const std::string delimiter = "===";

}

bool readDictionary(std::ifstream& inputFile, TextSpellChecker& checker, size_t& readLineNumber)
{
   for (; ; ++readLineNumber)
   {
      if (readLineNumber > gc_maxLinesInFile)
      {
         std::cout << "Too large file, max " << gc_maxLinesInFile << " lines allowed\n";
         return false;
      }
      std::string line;
      std::getline(inputFile, line);
      if (line == delimiter)
      {
         break;
      }
      if (!inputFile)
      {
         std::cout << "End of file reached while delimiter " << delimiter << " not read\n";
         return false;
      }
      std::istringstream iss(line);
      std::vector<std::string> dictionary;
      std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(dictionary));

      for (const auto& word : dictionary)
      {
         if (word.size() > gc_maxWordLength)
         {
            std::cout << "Too long word: " << word << " , max " << gc_maxWordLength << " chars allowed\n";
            return false;
         }
         checker.AddWordToDictionary(word);
      }
   }

   return true;
}

bool readText(std::ifstream& inputFile, std::string& textToCheck, size_t& readLineNumber)
{
   for (; ; ++readLineNumber)
   {
      if (readLineNumber > gc_maxLinesInFile)
      {
         std::cout << "Too large file, max " << gc_maxLinesInFile << " lines allowed\n";
         return false;
      }
      std::string line;
      std::getline(inputFile, line);
      if (line == delimiter)
      {
         break;
      }
      if (!inputFile)
      {
         std::cout << "End of file reached while delimiter " << delimiter << " not read\n";
         return false;
      }
      textToCheck += line;
      textToCheck += '\n';
   }

   return true;
}

bool processInputArgs(int argc, char* argv[],
   std::ofstream& outputFile, TextSpellChecker& checker, std::string& textToCheck)
{
   if (argc != 3)
   {
      std::cout << "spell-checker <input> <output>\n";
      return false;
   }

   std::ifstream inputFile(argv[1]);
   if (!inputFile.is_open())
   {
      std::cout << argv[1] << " can't be opened\n";
      return false;
   }

   outputFile.open(argv[2]);
   if (!outputFile.is_open())
   {
      std::cout << argv[2] << " can't be opened\n";
      return false;
   }

   size_t readLineNumber = 0;
   if (!readDictionary(inputFile, checker, readLineNumber))
   {
      return false;
   }

   if (!readText(inputFile, textToCheck, readLineNumber))
   {
      return false;
   }

   return true;
}

int main(int argc, char* argv[])
{
   std::ofstream outputFile;
   TextSpellChecker checker;
   std::string textToCheck;
   if (!processInputArgs(argc, argv, outputFile, checker, textToCheck))
   {
      return -1;
   }

   std::string output = checker.CheckText(textToCheck);
   outputFile.write(&output[0], output.size());

   return 0;
}
