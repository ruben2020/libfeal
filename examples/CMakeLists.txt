#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
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

if (CURL_EX)
add_subdirectory( "promise_curl" )
endif (CURL_EX)
