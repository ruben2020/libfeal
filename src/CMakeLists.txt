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
  )

add_library( feal_lib STATIC ${FEAL_LIB_SRCS} )

if(WIN32)
target_link_libraries( feal_lib Threads::Threads ws2_32 )
else(WIN32)
target_link_libraries( feal_lib Threads::Threads )
endif(WIN32)
