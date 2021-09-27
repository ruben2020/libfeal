
cmake_minimum_required(VERSION 3.2.2)
project(libfeal)

find_package(Threads REQUIRED)

include_directories( "../include" )

set( FEAL_LIB_SRCS 
     FealCommon.cpp
     FealEvent.cpp
     FealEventBus.cpp
     FealTimer.cpp
     FealActor.cpp
     FealActorVector.cpp
     FealBaseSignal.cpp
     FealSignal.cpp
     FealDatagram.cpp
  )

add_library( feal_lib STATIC ${FEAL_LIB_SRCS} )

if(WIN32)
target_link_libraries( feal_lib Threads::Threads ws2_32 )
else(WIN32)
target_link_libraries( feal_lib Threads::Threads )
endif(WIN32)
