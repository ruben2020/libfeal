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
project(udp_echo_clientserver)

include_directories( "." )
include_directories( "../../include" )

set( UDP_SERVER_SRCS 
    server.cpp
    main_server.cpp
  )

set( UDP_CLIENT_SRCS 
    client.cpp
    main_client.cpp
  )

set( UDP_SERVER6_SRCS 
    server.cpp
    server6.cpp
    main_server6.cpp
  )

set( UDP_CLIENT6_SRCS 
    client.cpp
    client6.cpp
    main_client6.cpp
  )

add_executable( fealudpserver ${UDP_SERVER_SRCS} )
target_link_libraries( fealudpserver feal_lib )

add_executable( fealudpclient ${UDP_CLIENT_SRCS} )
target_link_libraries( fealudpclient feal_lib )

add_executable( fealudpserver6 ${UDP_SERVER6_SRCS} )
target_link_libraries( fealudpserver6 feal_lib )

add_executable( fealudpclient6 ${UDP_CLIENT6_SRCS} )
target_link_libraries( fealudpclient6 feal_lib )


