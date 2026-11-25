cmake_minimum_required(VERSION 3.28.3)

project(cyrus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CYRUS_STANDALONE "Build against the headless Qt vendored in third_party/qt6 instead of the system Qt" OFF)
option(CYRUS_BUILD_TESTS "Build the Catch2 unit tests for the model and controller layers" ON)

include(cmake/dependencies.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(asset)
add_subdirectory(src)

if(CYRUS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
