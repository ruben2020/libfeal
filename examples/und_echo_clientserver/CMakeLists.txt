
cmake_minimum_required(VERSION 3.2.2)
project(und_echo_clientserver)

include_directories( "." )
include_directories( "../../include" )

set( UND_SERVER_SRCS 
    serverund.cpp
    main_serverund.cpp
  )

set( UND_CLIENT_SRCS 
    clientund.cpp
    main_clientund.cpp
  )

add_executable( fealundserver ${UND_SERVER_SRCS} )
target_link_libraries( fealundserver feal_lib )

add_executable( fealundclient ${UND_CLIENT_SRCS} )
target_link_libraries( fealundclient feal_lib )

