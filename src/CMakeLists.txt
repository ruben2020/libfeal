#
# Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.20.0)
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
     FealBaseStream.cpp
     FealStream.cpp
     FealDatagram.cpp
     FealDescMon.cpp
  )

set( FEAL_LIB_UNIX_SRCS 
     FealReader.cpp
     FealFileDirMon.cpp
  )

if(WIN32)
add_library( feal_lib STATIC ${FEAL_LIB_SRCS} )
target_link_libraries( feal_lib Threads::Threads ws2_32 )
else(WIN32)
add_library( feal_lib STATIC ${FEAL_LIB_SRCS} ${FEAL_LIB_UNIX_SRCS})
target_link_libraries( feal_lib Threads::Threads )
endif(WIN32)
