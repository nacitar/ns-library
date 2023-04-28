cmake_minimum_required(VERSION 3.21)

project(ns VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    ns_core
    GIT_REPOSITORY https://github.com/nacitar/ns-core-library.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ns_core)

enable_testing()

add_custom_target(
  ns_vcs_metadata_updater
  COMMAND ${CMAKE_COMMAND}
  -D SRC=${CMAKE_CURRENT_SOURCE_DIR}/config/ns/vcs/metadata.hpp.in
  -D DST=${CMAKE_CURRENT_BINARY_DIR}/generated/ns/vcs/metadata.hpp
  -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/update_vcs_metadata.cmake
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Configuring ns VCS metadata"
  VERBATIM
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ns/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/ns/version.hpp
    @ONLY
)

add_library(ns STATIC)
add_dependencies(ns ns_vcs_metadata_updater)
target_include_directories(
    ns
    INTERFACE include
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(ns PUBLIC ns_core)
file(GLOB_RECURSE NS_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.cxx)
target_sources(ns PRIVATE ${NS_SOURCES})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ns PRIVATE
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
    target_compile_options(ns PRIVATE
        /W4
        /WX
        /O2
        /EHsc
    )
endif()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp tests/*.cxx)
foreach(test_src IN LISTS TEST_SOURCES)
  get_filename_component(test_base_name ${test_src} NAME_WE)
  set(test_name ns_${test_base_name})  # namespace it
  add_executable(${test_name} ${test_src})
  target_link_libraries(${test_name} ns)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
