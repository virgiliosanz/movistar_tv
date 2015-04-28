cmake_minimum_required(VERSION 2.6)

set(PROJECT_NAME movistart_epg)
set(PROJ_VERSION 0.0.1)

project(${PROJECT_NAME})
enable_testing()
set(CMAKE_VERBOSE_MAKEFILE 0)

# Enabke debug mode if not release is comming
if (NOT CMAKE_BUILD_TYPE MATCHES release)
    set(CMAKE_BUILD_TYPE debug)
endif()
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

# Make compaining compiler if gcc
add_definitions(-Wall -Wextra -std=gnu99 -D_GNU_SOURCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

# Add thread support
#find_package(Threads)

# Add Include directories
include_directories(src/include)

# Libraries being used
find_package(PkgConfig REQUIRED)
pkg_check_modules(THIRDPARTY REQUIRED libxml-2.0 yajl)
include_directories(${THIRDPARTY_INCLUDE_DIRS})
link_directories(${THIRDPARTY_LIBRARY_DIRS})
add_definitions(${THIRDPARTY_OTHERS})

# Copy (or set path for data)

## Install: http://www.cmake.org/Wiki/CMake:CPackPackageGenerators
#INSTALL(TARGETS main main DESTINATION distance)
#SET(CPACK_GENERATOR "DEB")
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Virgilio Sanz")
#INCLUDE(CPack)

# Source code dirs
add_subdirectory(src)
