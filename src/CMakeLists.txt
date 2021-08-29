
cmake_minimum_required(VERSION 3.2.2)
project(libfeal)

find_package(Threads REQUIRED)

include_directories( "../include" )

set( FEAL_LIB_SRCS 
     FealEvent.cpp
     FealEventBus.cpp
     FealTimer.cpp
     FealActor.cpp
     FealActorVector.cpp
     # FealStream.cpp
     # FealTCPStream.cpp
  )

add_library( feal_lib STATIC ${FEAL_LIB_SRCS} )
target_link_libraries( feal_lib Threads::Threads )
