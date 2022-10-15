#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.2.2)
project(feal)

# C++11 required
SET( CMAKE_CXX_STANDARD 11 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_EXTENSIONS OFF )

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-defaulted-function-deleted")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
else(  )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif( )

add_subdirectory( "src" )
add_subdirectory( "test" )
add_subdirectory( "examples" )
