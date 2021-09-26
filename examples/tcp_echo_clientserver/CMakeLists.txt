
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


