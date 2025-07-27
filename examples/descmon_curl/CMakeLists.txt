#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
project(curlmulti)

find_package(CURL REQUIRED) 

include_directories( "." )
include_directories( "../../include" )
include_directories(${CURL_INCLUDE_DIR})

set( CURLMULTI_SRCS 
    ActorA.cpp
    main_curlmulti.cpp
  )

add_executable( fealcurlmulti ${CURLMULTI_SRCS} )
target_link_libraries( fealcurlmulti feal_lib ${CURL_LIBRARIES})

