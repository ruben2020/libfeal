#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.20.0)
project(feal)

option(CURL_EX    "libCurl example" OFF)
option(OPENSSL_EX "OpenSSL example" OFF)
option(GHAWIN "Building for Windows in Github Actions" OFF)

MESSAGE("-- libCurl = " ${CURL_EX})
MESSAGE("-- OpenSSL = " ${OPENSSL_EX})
MESSAGE("-- GHAWIN = " ${GHAWIN})

# C++17 required
SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_EXTENSIONS OFF )

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-defaulted-function-deleted")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-defaulted-function-deleted")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
else(  )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif( )

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

add_subdirectory( "src" )
add_subdirectory( "test" )
add_subdirectory( "examples" )
