#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
project(fealpromise)

include_directories( "." )
include_directories( "../../include" )

set( PROMISE_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_promise.cpp
  )

add_executable( fealpromise ${PROMISE_SRCS} )
target_link_libraries( fealpromise feal_lib )

