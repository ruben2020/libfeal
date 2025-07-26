#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
project(notifymon)

include_directories( "." )
include_directories( "../../include" )

set( INOTIFYMON_SRCS 
    ActorA.cpp
    main_inotify.cpp
  )

add_executable( inotifymon ${INOTIFYMON_SRCS} )
target_link_libraries( inotifymon feal_lib )

