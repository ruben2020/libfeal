#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

cmake_minimum_required(VERSION 3.16.0)
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

