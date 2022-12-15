#
# Copyright (c) 2022 ruben2020 https://github.com/ruben2020
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

cmake_minimum_required(VERSION 3.2.2)
project(pipefifo)

include_directories( "." )
include_directories( "../../include" )

set( FIFO_READ_SRCS 
    fiforeader.cpp
    main_fiforeader.cpp
  )

set( FIFO_WRITE_SRCS 
    fifowriter.cpp
    main_fifowriter.cpp
  )

add_executable( fealfiforeader ${FIFO_READ_SRCS} )
target_link_libraries( fealfiforeader feal_lib )

add_executable( fealfifowriter ${FIFO_WRITE_SRCS} )
target_link_libraries( fealfifowriter feal_lib )

