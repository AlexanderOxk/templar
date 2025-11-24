cmake_minimum_required(VERSION 3.20)

project(
  templar
  VERSION 0.1
  LANGUAGES CXX
)

option(TEMPLAR_INCLUDE_TESTS "Include test targets in the build" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(src)

if(TEMPLAR_INCLUDE_TESTS)
  add_subdirectory(tests)
endif()
