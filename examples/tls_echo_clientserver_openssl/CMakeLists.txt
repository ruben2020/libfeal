#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.20.0)
project(tls_echo_clientserver_openssl)

set(OPENSSL_USE_STATIC_LIBS FALSE)
find_package(OpenSSL REQUIRED)

include_directories( "." )
include_directories( "../../include" )

if (GHAWIN)
include_directories( ${GHA_SSLINCL} )
else (GHAWIN)
include_directories( OPENSSL_INCLUDE_DIR )
endif (GHAWIN)


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
add_executable( fealtlsclient ${TCP_CLIENT_SRCS} )
add_executable( fealtlsserver6 ${TCP_SERVER6_SRCS} )
add_executable( fealtlsclient6 ${TCP_CLIENT6_SRCS} )

if (GHAWIN)
target_link_libraries( fealtlsserver PRIVATE feal_lib ${GHA_LIBSSL} ${GHA_LIBCRYPTO})
target_link_libraries( fealtlsclient PRIVATE feal_lib ${GHA_LIBSSL} ${GHA_LIBCRYPTO})
target_link_libraries( fealtlsserver6 PRIVATE feal_lib ${GHA_LIBSSL} ${GHA_LIBCRYPTO})
target_link_libraries( fealtlsclient6 PRIVATE feal_lib ${GHA_LIBSSL} ${GHA_LIBCRYPTO})
else (GHAWIN)
target_link_libraries( fealtlsserver PRIVATE feal_lib OpenSSL::SSL)
target_link_libraries( fealtlsclient PRIVATE feal_lib OpenSSL::SSL)
target_link_libraries( fealtlsserver6 PRIVATE feal_lib OpenSSL::SSL)
target_link_libraries( fealtlsclient6 PRIVATE feal_lib OpenSSL::SSL)
endif (GHAWIN)
