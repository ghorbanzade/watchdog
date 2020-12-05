#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(Watchdog VERSION 1.0.0 LANGUAGES CXX)

include(CTest)

option(BUILD_SHARED_LIBS "build watchdog as a shared library" OFF)
option(WATCHDOG_BUILD_TESTS "build unit-tests" ON)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CONAN_CMAKE_SILENT_OUTPUT ON)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "_${CMAKE_BUILD_TYPE}")
endif()

if (NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

if (WATCHDOG_BUILD_TESTS)
    if (MSVC)
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wfatal-errors")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

macro(watchdog_find_package package_name)
    find_package(${package_name} QUIET)
    if (NOT ${package_name}_FOUND)
        message (FATAL_ERROR "required dependency on ${package_name} is missing")
    endif()
endmacro()

set(WATCHDOG_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(WATCHDOG_INCLUDE_DIR "${WATCHDOG_ROOT_DIR}/include")
set(WATCHDOG_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")

add_subdirectory(src)
add_subdirectory(tools/frontend)
add_subdirectory(tools/service)

if (WATCHDOG_BUILD_TESTS)
    add_subdirectory(tests)
endif()
