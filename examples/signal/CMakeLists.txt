#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR LGPL-2.1-or-later
#

cmake_minimum_required(VERSION 3.2.2)
project(fealsignal)

include_directories( "." )
include_directories( "../../include" )

set( SIGNAL_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_signal.cpp
  )

add_executable( fealsignal ${SIGNAL_SRCS} )
target_link_libraries( fealsignal feal_lib )

