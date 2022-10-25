#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.2.2)
project(fealexamples)

add_subdirectory( "actors_and_timers" )
add_subdirectory( "tcp_echo_clientserver" )
add_subdirectory( "udp_echo_clientserver" )
add_subdirectory( "signal" )
add_subdirectory( "promise" )
if(WIN32)
else(WIN32)
add_subdirectory( "uns_echo_clientserver" )
add_subdirectory( "und_echo_clientserver" )
add_subdirectory( "uns_echo_clientserver_abstract" )
add_subdirectory( "sigchld_fork" )
add_subdirectory( "pipe_fifo" )
add_subdirectory( "fork_ipc" )
endif(WIN32)
