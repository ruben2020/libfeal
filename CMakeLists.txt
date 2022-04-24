#
#  Copyright 2021 ruben2020 https://github.com/ruben2020
#
#  Licensed under the Apache License, Version 2.0
#  with LLVM Exceptions (the "License");
#  you may not use this file except in compliance with the License.
#  You can find a copy of the License in the LICENSE file.
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#  
#  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
