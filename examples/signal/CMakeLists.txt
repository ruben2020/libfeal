#
#  Copyright 2021 ruben2020 https://github.com/ruben2020
#
#  Licensed under the Apache License, Version 2.0
#  with LLVM Exceptions (the "License");
#  you may not use this file except in compliance with the License.
#  You can find a copy of the License in the LICENSE file.
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#  
#  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#  

cmake_minimum_required(VERSION 3.2.2)
project(fealsignal)

include_directories( "." )
include_directories( "../../include" )

set( SIGNAL_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_signal.cpp
  )

add_executable( fealsignal ${SIGNAL_SRCS} )
target_link_libraries( fealsignal feal_lib )

