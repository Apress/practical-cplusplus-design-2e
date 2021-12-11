# Don't forget to set environment variable CMAKE_PREFIX_PATH to Qt installed directory
cmake_minimum_required(VERSION 3.16.3)
project(pdCalc)

#
# cmake setup
#
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

#default to building Debug if not set
#if(CMAKE_BUILD_TYPE STREQUAL "")
#    set(CMAKE_BUILD_TYPE "Debug")
#endif()

#if(CMAKE_BUILD_TYPE STREQUAL "None")
#    set(CMAKE_BUILD_TYPE "Debug")
#endif()

#
# Qt setup
#
if(NOT DEFINED PDCALC_QT_VERSION)
    message(WARNING "PDCALC_QT_VERSION must be 5 or 6 - defaulting to 5 - ensure CMAKE_PREFIX_PATH matches")
    set(PDCALC_QT_VERSION 5)
endif()

set(QT_V ${PDCALC_QT_VERSION})

#
# pdCalc setup
#

# Handle version number
set(VERSION_MAJOR 2)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(PDCALC_VERSION_STR "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
add_definitions(-DPDCALC_VERSION="${PDCALC_VERSION_STR}")
add_definitions(-DTEST_DIR="../test")

# set paths
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#
# Preprocessor defines
#

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
endif()

if(UNIX)
    add_definitions(-DPOSIX)
elseif(WIN32)
    add_definitions(-DWIN32)
endif()

#
# C++ and compiler options
#

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -fcoroutines -fmodules-ts")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -fmodules-ts")
    #add_definitions(-DUSING_RANGE_V3)
    #include_directories("/home/absinger/code/range-v3/include")
    # option to set toolchain to different gcc:
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--gcc-toolchain=/home/absinger/code/gcc/bin/")
  # using Clang
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
  add_definitions(-D_USE_MATH_DEFINES)
  include_directories(${PROJECT_SOURCE_DIR})
endif()


function(denote_module_interface)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        foreach(f ${ARGV})
            set_source_files_properties(${f} PROPERTIES COMPILE_FLAGS /interface)
        endforeach()
    endif()
endfunction()

#
# test setup and cmake recursive descent
#
set(BUILD_TEST TRUE CACHE BOOL "Flag for building test or not")

if(BUILD_TEST)
    add_definitions(-DENABLE_TESTING_INTERFACE)
endif()

add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(test)
endif()
