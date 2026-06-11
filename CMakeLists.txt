cmake_minimum_required(VERSION 3.15)

project(
  cooperative_scheduler
  VERSION 1.1.1
  DESCRIPTION "C++17 Header only cooperative scheduler"
  HOMEPAGE_URL "https://github.com/snhobbs/CooperativeScheduler"
  LANGUAGES CXX
)

add_library(cooperative_scheduler INTERFACE)
add_library(cooperative_scheduler::cooperative_scheduler ALIAS cooperative_scheduler)

target_include_directories(
  cooperative_scheduler INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(_cs_tests_default ON)
else()
  set(_cs_tests_default OFF)
endif()
option(COOPERATIVE_SCHEDULER_BUILD_TESTS "Build CooperativeScheduler tests"
       ${_cs_tests_default})

if(COOPERATIVE_SCHEDULER_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
