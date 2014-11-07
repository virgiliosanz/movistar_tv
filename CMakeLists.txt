cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME movistart_epg)
set(PROJ_VERSION 0.0.1)
project(${PROJECT_NAME})
enable_testing()

set(CMAKE_VERBOSE_MAKEFILE 0)


if(CMAKE_COMPILER_IS_GNUCC)
  add_definitions(-Wall -Werror -pedantic -std=c99)
endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)

