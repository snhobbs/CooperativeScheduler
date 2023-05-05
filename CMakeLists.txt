cmake_minimum_required(VERSION 3.15)
#set(ENV{LSAN_OPTIONS} "verbosity=1:log_threads=1")
set(TargetName cooperative_scheduler)
set(COOPERATIVE_SCHEDULER_MAJOR_VERSION 1)
set(COOPERATIVE_SCHEDULER_MINOR_VERSION 1)
set(COOPERATIVE_SCHEDULER_PATCH_VERSION 1)
set(PACKAGE_VERSION ${COOPERATIVE_SCHEDULER_MAJOR_VERSION}.${COOPERATIVE_SCHEDULER_MINOR_VERSION}.${COOPERATIVE_SCHEDULER_PATCH_VERSION})

project(
  ${TargetName}
  VERSION ${PACKAGE_VERSION}
  DESCRIPTION "C++17 Header only cooperative scheduler"
  HOMEPAGE_URL "https://github.com/snhobbs/CooperativeScheduler"
  LANGUAGES CXX
)

add_library(${TargetName} INTERFACE)
add_library(${TargetName}::${TargetName} ALIAS ${TargetName})

target_include_directories(
  ${TargetName} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(PACKAGE_NAME ${TargetName})
string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

#set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
#configure_file(PackageConfig.cmake.in
#  "${PROJECT_BINARY_DIR}/${TargetName}Config.cmake" @ONLY)

#configure_file(PackageConfigVersion.cmake.in
#  "${PROJECT_BINARY_DIR}/${TargetName}ConfigVersion.cmake" @ONLY)

#-----------------------------------------------------------------------------
add_subdirectory(tests)

