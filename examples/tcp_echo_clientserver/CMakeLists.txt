#
#  Copyright 2021 ruben2020 https://github.com/ruben2020
#
#  Licensed under the Apache License, Version 2.0
#  with LLVM Exceptions (the "License");
#  you may not use this file except in compliance with the License.
#  You can find a copy of the License in the LICENSE file.
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#  
#  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#  

cmake_minimum_required(VERSION 3.2.2)
project(tcp_echo_clientserver)

include_directories( "." )
include_directories( "../../include" )

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

add_executable( fealtcpserver ${TCP_SERVER_SRCS} )
target_link_libraries( fealtcpserver feal_lib )

add_executable( fealtcpclient ${TCP_CLIENT_SRCS} )
target_link_libraries( fealtcpclient feal_lib )

add_executable( fealtcpserver6 ${TCP_SERVER6_SRCS} )
target_link_libraries( fealtcpserver6 feal_lib )

add_executable( fealtcpclient6 ${TCP_CLIENT6_SRCS} )
target_link_libraries( fealtcpclient6 feal_lib )


