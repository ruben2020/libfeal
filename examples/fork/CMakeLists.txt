#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.2.2)
project(fork)

include_directories( "." )
include_directories( "../../include" )

set( FORK_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_fork.cpp
  )

add_executable( fealfork ${FORK_SRCS} )
target_link_libraries( fealfork feal_lib )

