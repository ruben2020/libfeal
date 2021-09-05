
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

add_executable( fealtcpserver ${TCP_SERVER_SRCS} )
target_link_libraries( fealtcpserver feal_lib )

add_executable( fealtcpclient ${TCP_CLIENT_SRCS} )
target_link_libraries( fealtcpclient feal_lib )


