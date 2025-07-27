#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
project(filedirmon)

include_directories( "." )
include_directories( "../../include" )

set( FILEDIRMON_SRCS 
    ActorA.cpp
    main_filedirmon.cpp
  )

add_executable( fealfiledirmon ${FILEDIRMON_SRCS} )
target_link_libraries( fealfiledirmon feal_lib )

