#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.20.0)
project(fealpromisecurl)

find_package(CURL REQUIRED) 

include_directories( "." )
include_directories( "../../include" )
include_directories(${CURL_INCLUDE_DIR})

set( PROMISE_CURL_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_promise.cpp
  )

add_executable( fealpromisecurl ${PROMISE_CURL_SRCS} )
target_link_libraries( fealpromisecurl feal_lib ${CURL_LIBRARIES})
