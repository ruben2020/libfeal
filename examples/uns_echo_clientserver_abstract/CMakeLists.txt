#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

cmake_minimum_required(VERSION 3.16.0)
project(uns_echo_clientserver_abstract)

include_directories( "." )
include_directories( "../../include" )
include_directories( "../tcp_echo_clientserver" )

set( UNS_SERVER_ABS_SRCS 
    ../tcp_echo_clientserver/clienthandler.cpp
    ../tcp_echo_clientserver/server.cpp
    serveruns.cpp
    main_serveruns.cpp
  )

set( UNS_CLIENT_ABS_SRCS 
    ../tcp_echo_clientserver/client.cpp
    clientuns.cpp
    main_clientuns.cpp
  )

add_executable( fealunsserver_abs ${UNS_SERVER_ABS_SRCS} )
target_link_libraries( fealunsserver_abs feal_lib )

add_executable( fealunsclient_abs ${UNS_CLIENT_ABS_SRCS} )
target_link_libraries( fealunsclient_abs feal_lib )

