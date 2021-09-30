
cmake_minimum_required(VERSION 3.2.2)
project(fealpromise)

include_directories( "." )
include_directories( "../../include" )

set( PROMISE_SRCS 
    ActorsManager.cpp
    ActorA.cpp
    main_promise.cpp
  )

add_executable( fealpromise ${PROMISE_SRCS} )
target_link_libraries( fealpromise feal_lib )

