#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR LGPL-2.1-or-later
#

cmake_minimum_required(VERSION 3.2.2)
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

