#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

cmake_minimum_required(VERSION 3.16.0)
project(und_echo_clientserver)

include_directories( "." )
include_directories( "../../include" )

set( UND_SERVER_SRCS 
    serverund.cpp
    main_serverund.cpp
  )

set( UND_CLIENT_SRCS 
    clientund.cpp
    main_clientund.cpp
  )

add_executable( fealundserver ${UND_SERVER_SRCS} )
target_link_libraries( fealundserver feal_lib )

add_executable( fealundclient ${UND_CLIENT_SRCS} )
target_link_libraries( fealundclient feal_lib )

