# Axel '0vercl0k' Souchet - April 18 2020
# CMakeList.txt : CMake project for kdmp-parser, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

OPTION(BUILD_TESTS "Build and run tests" OFF)

project(kdmp-parser)
add_subdirectory(src/lib)
add_subdirectory(src/parser)
add_subdirectory(src/testapp)
if(BUILD_TESTS)
    add_subdirectory(src/tests)
endif()