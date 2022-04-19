#
#  Copyright 2021 ruben2020 https://github.com/ruben2020
#
#  Dual-licensed under the Apache License, Version 2.0, and
#  the GNU General Public License, Version 2.0;
#  you may not use this file except in compliance
#  with either one of these licenses.
#
#  You can find copies of these licenses in the included
#  LICENSE-APACHE2 and LICENSE-GPL2 files.
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under these licenses is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the license texts for the specific language governing permissions
#  and limitations under the licenses.
#
#  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
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
