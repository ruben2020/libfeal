#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

cmake_minimum_required(VERSION 3.16.0)
project(fealactorstimers)

include_directories( "." )
include_directories( "../../include" )

set( ACTORSTIMERS_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    ActorB.cpp
    main_actorstimers.cpp
  )

add_executable( fealactorstimers ${ACTORSTIMERS_SRCS} )
target_link_libraries( fealactorstimers feal_lib )

