
cmake_minimum_required(VERSION 3.2.2)
project(feal)

# C++11 required
SET( CMAKE_CXX_STANDARD 11 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_EXTENSIONS OFF )

add_subdirectory( "src" )
add_subdirectory( "test" )
add_subdirectory( "examples" )

