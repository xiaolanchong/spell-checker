cmake_minimum_required(VERSION 3.18)

project(spell-checker-cm)

add_subdirectory(test)

set(headers
   Trie.h
   WordSpellChecker.h
   TextSpellChecker.h
   )

set(sources
   Trie.cpp
   WordSpellChecker.cpp
   TextSpellChecker.cpp
   spell-checker.cpp
   )


add_executable(${PROJECT_NAME} ${headers} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)