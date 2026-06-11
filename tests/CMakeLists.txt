include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0)
set(INSTALL_GTEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(
  tests
  source/main.cpp
  source/test_CooperativeScheduler.cpp
  source/test_StaticCooperativeScheduler.cpp)

set_property(TARGET tests PROPERTY CXX_STANDARD 17)
target_compile_definitions(tests PRIVATE TESTING UNITTESTING LINUX)
target_compile_options(
  tests
  PRIVATE -O2
          -fsanitize=address
          -fsanitize=undefined
          -fno-omit-frame-pointer
          -Wall
          -Wextra
          -Wpedantic
          -Wconversion
          -Wshadow
          $<$<CXX_COMPILER_ID:GNU>:-Wsuggest-final-types>
          $<$<CXX_COMPILER_ID:GNU>:-Wmaybe-uninitialized>
          $<$<CXX_COMPILER_ID:GNU>:-Wdouble-promotion>
          $<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>
          $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
          $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches>
          $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
          $<$<CXX_COMPILER_ID:GNU>:-Wrestrict>
          $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>)
target_link_options(tests PRIVATE -fsanitize=address -fsanitize=undefined)
target_link_libraries(tests PRIVATE GTest::gtest
                                    cooperative_scheduler::cooperative_scheduler
                                    pthread)
target_include_directories(tests PRIVATE source)

gtest_discover_tests(tests)
