
cmake_minimum_required(VERSION 3.2.2)
project(fealhelloworld)

include_directories( "." )
include_directories( "../../include" )

set( HELLO_WORLD_SRCS 
    HelloWorld.cpp
    ActorA.cpp
    ActorB.cpp
  )

add_executable( fealhelloworld ${HELLO_WORLD_SRCS} )
target_link_libraries( fealhelloworld feal_lib )

