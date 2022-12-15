#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

cmake_minimum_required(VERSION 3.2.2)
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
