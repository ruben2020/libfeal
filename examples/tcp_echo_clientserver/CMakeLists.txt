#
#  Copyright 2021 ruben2020 https://github.com/ruben2020
#
#  Dual-licensed under the Apache License, Version 2.0, and
#  the GNU General Public License, Version 2.0;
#  you may not use this file except in compliance
#  with either one of these licenses.
#
#  You can find copies of these licenses in the included
#  LICENSE-APACHE2 and LICENSE-GPL2 files.
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under these licenses is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the license texts for the specific language governing permissions
#  and limitations under the licenses.
#
#  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
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


