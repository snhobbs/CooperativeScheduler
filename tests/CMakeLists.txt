cmake_minimum_required(VERSION 3.10.0)

project(Testing)
enable_testing()

set(TargetName ${PROJECT_NAME})
set(dirs
 CooperativeScheduler
)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(source_directories
  ${CMAKE_CURRENT_SOURCE_DIR}/source
)


#-----------------------------------------------------------------------------

add_executable(${TargetName})

option(Sanitizer "Run address sanitizer" OFF)

if(${Sanitizer})
  target_compile_options(
    ${TargetName}
    PRIVATE
    $<$<CONFIG:DEBUG>:-fsanitize=address>
    -fno-omit-frame-pointer
    $<$<CONFIG:DEBUG>:-fsanitize=undefined>
  )
  target_link_libraries(${TargetName} asan)
  #target_link_libraries(${TargetName} tsan)
  target_link_libraries(${TargetName} ubsan)
  #target_link_libraries(${TargetName} msan)
endif()

target_compile_options(
  ${TargetName}
  PRIVATE
    $<$<CONFIG:DEBUG>:-g3>
    -Wall
    -Wextra
    -pedantic
    -Wpedantic
    #-pedantic-errors
    #-Werror=pedantic
    -Wno-float-conversion

    -Wconversion
    $<$<COMPILE_LANGUAGE:C>:-Wimplicit>
    $<$<COMPILE_LANGUAGE:C>:-Wunused>

    #$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    #$<$<COMPILE_LANGUAGE:CXX>:-Wabi>
    $<$<CXX_COMPILER_ID:GNU>:-Wsuggest-final-types>
    $<$<CXX_COMPILER_ID:GNU>:-Wmaybe-uninitialized>
    $<$<CXX_COMPILER_ID:GNU>:-Wdouble-promotion>
    $<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>
    -Winit-self

    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches>
    $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
    $<$<CXX_COMPILER_ID:GNU>:-Wrestrict>
    $<$<C_COMPILER_ID:GNU>:$<$<COMPILE_LANGUAGE:C>:-Wjump-misses-init>>
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wshadow
)

target_compile_features(${TargetName} PUBLIC cxx_std_17)
target_compile_definitions(${TargetName} PRIVATE TESTING)
target_compile_definitions(${TargetName} PRIVATE UNITTESTING)
target_compile_definitions(${TargetName} PRIVATE LINUX)
set_property(TARGET ${TargetName} PROPERTY CXX_STANDARD 17)
SET(CMAKE_VERBOSE_MAKEFILE ON)

#fixme this should have all the sources
foreach(dir ${source_directories})
  aux_source_directory(${dir} DIR_SRCS)
  message("${dir}")
endforeach()

foreach(src ${DIR_SRC})
  message("${src}")
endforeach()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source DIR_SRCS)
target_sources(${TargetName} PUBLIC ${DIR_SRCS})
#target_sources(${TargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)

get_target_property( srcs ${TargetName} SOURCES)
foreach(src ${srcs})
  message("Source: ${src}")
endforeach()

find_package(gtest QUIET)
target_link_libraries(${TargetName} gtest)
target_link_libraries(${TargetName} pthread)

target_include_directories(${TargetName} PUBLIC ${TESTSRC})
target_include_directories(${TargetName} PUBLIC ${PROJECT_SOURCE_DIR}/include)

message(STATUS "Sources: ${DIR_SRCS}")
add_test(NAME ${CMAKE_SOURCE_DIR} COMMAND ${TargetName})

#------------------------------------------------------------
# Static Analysis
#------------------------------------------------------------
function(Analyze source_directories)
  message(Running Analysis)
  list(APPEND AnalyseFiles ${source_directories})
  include(CMakeStaticAnalysis/Utilities.cmake)
  include(CMakeStaticAnalysis/StaticAnalysis.cmake)
  GetStaticAnalysisFiles("${TargetName}" "")
  message("${AnalyseFiles}")
  set(CMAKE_CXX_CPPLINT "cpplint;--verbose=5;--linelength=100")
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*;-format-style=google;-export-fixes=${CMAKE_CURRENT_BINARY_DIR}/clangtidy.results")

  Cpplint("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" cpplint)
  Cppcheck("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" cppcheck)
  flint("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" flint++)
endfunction()

option(RUN_ANALYSIS "Run CppCheck, CppLint, and Clang-tidy" OFF)
if(RUN_ANALYSIS)
  Analyze("${DIR_SRCS}")
endif()
