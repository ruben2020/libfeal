#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
project(forkipc)

include_directories( "." )
include_directories( "../../include" )

set( FORK_IPC_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_fork.cpp
  )

add_executable( fealforkipc ${FORK_IPC_SRCS} )
target_link_libraries( fealforkipc feal_lib )

