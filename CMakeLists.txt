cmake_minimum_required(VERSION 3.21)

project(ns VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    ns-core
    GIT_REPOSITORY https://github.com/nacitar/ns-core-library.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ns-core)

find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    ERROR_QUIET
    OUTPUT_VARIABLE COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} symbolic-ref -q --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    ERROR_QUIET
    OUTPUT_VARIABLE BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()
if(NOT COMMIT_HASH)
  set(COMMIT_HASH "unknown")
endif()
if(NOT BRANCH_NAME)
  set(BRANCH_NAME "[detached]")
endif()

configure_file(config/version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/ns/version.h @ONLY)

file(GLOB_RECURSE NS_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.cxx)

add_library(ns INTERFACE)
target_include_directories(
    ns
    INTERFACE include
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated
)

# Static library used so that the build script works even if header only.
if(NS_SOURCES)
  add_library(ns-impl STATIC ${NS_SOURCES})
  target_link_libraries(ns INTERFACE ns-impl ns-core)

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(ns-impl PRIVATE
          -Wall
          -Wextra
          -Wpedantic
          -Werror
          -Wold-style-cast
          #-Wno-unused-function
          #-Wno-unused-value
          -Wconversion
          -Wshadow
          -Wsign-conversion
          -Wnon-virtual-dtor
          -O3
      )
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      target_compile_options(ns-impl PRIVATE
          /W4
          /WX
          /O2
          /EHsc
      )
  endif()
endif()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp tests/*.cxx)

foreach(test_src IN LISTS TEST_SOURCES)
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src})
  target_link_libraries(${test_name} ns)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
