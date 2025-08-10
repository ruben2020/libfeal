#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16.0)
project(tls_echo_clientserver_openssl)

find_package(OpenSSL REQUIRED)

include_directories( "." )
include_directories( "../../include" )
include_directories( OPENSSL_INCLUDE_DIR )

set( TCP_SERVER_SRCS 
    clienthandler.cpp
    server.cpp
    main_server.cpp
  )

set( TCP_CLIENT_SRCS 
    client.cpp
    main_client.cpp
  )

set( TCP_SERVER6_SRCS 
    clienthandler.cpp
    server.cpp
    server6.cpp
    main_server6.cpp
  )

set( TCP_CLIENT6_SRCS 
    client.cpp
    client6.cpp
    main_client6.cpp
  )

add_executable( fealtlsserver ${TCP_SERVER_SRCS} )
target_link_libraries( fealtlsserver feal_lib OpenSSL::SSL)

add_executable( fealtlsclient ${TCP_CLIENT_SRCS} )
target_link_libraries( fealtlsclient feal_lib OpenSSL::SSL)

add_executable( fealtlsserver6 ${TCP_SERVER6_SRCS} )
target_link_libraries( fealtlsserver6 feal_lib OpenSSL::SSL)

add_executable( fealtlsclient6 ${TCP_CLIENT6_SRCS} )
target_link_libraries( fealtlsclient6 feal_lib OpenSSL::SSL)
