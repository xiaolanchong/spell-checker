cmake_minimum_required(VERSION 3.18)

project(spell-checker)

set(compiler_flags "-Wall" "-Wpedantic" "-Wextra")

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
if(UNIX)
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${compiler_flags})
  target_link_libraries(${PROJECT_NAME} "-lpthread")
endif()