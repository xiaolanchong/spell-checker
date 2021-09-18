#project(test)

set(headers
  ../Trie.h
  ../WordSpellChecker.h
  ../TextSpellChecker.h
  )

set(sources
  TrieTest.cpp
  SpellCheckerTest.cpp
  
  ../Trie.cpp
  ../WordSpellChecker.cpp
  ../TextSpellChecker.cpp
  
  ../googletest/googletest/src/gtest_main.cc
  ../googletest/googletest/src/gtest-all.cc
  )

include_directories(../googletest/googletest/include
                    ../googletest/googletest)
add_executable(test ${headers} ${sources})
target_compile_features(test PRIVATE cxx_std_17)
if(UNIX)
  target_link_libraries(test "-lpthread")
  set_target_properties(test PROPERTIES COMPILE_FLAGS ${compiler_flags})
endif()
