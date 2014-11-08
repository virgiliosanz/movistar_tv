cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME movistart_epg)
set(PROJ_VERSION 0.0.1)
project(${PROJECT_NAME})
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE 1)

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Werror -pedantic -std=c99i -static)
endif(CMAKE_COMPILER_IS_GNUCC)


if (NOT CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")


include_directories(src)

find_package(PkgConfig REQUIRED)
pkg_check_modules(THIRDPARTY REQUIRED yajl libxml-2.0 libcurl libpcre)
include_directories(${THIRDPARTY_INCLUDE_DIRS})
link_directories(${THIRDPARTY_LIBRARY_DIRS})
add_definitions(${THIRDPARTY_OTHERS})

add_subdirectory(src)
add_subdirectory(tests)
