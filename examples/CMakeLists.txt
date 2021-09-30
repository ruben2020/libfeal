
cmake_minimum_required(VERSION 3.2.2)
project(fealexamples)

add_subdirectory( "actors_and_timers" )
add_subdirectory( "tcp_echo_clientserver" )
add_subdirectory( "udp_echo_clientserver" )
add_subdirectory( "signal" )
add_subdirectory( "promise" )
if(WIN32)
else(WIN32)
add_subdirectory( "uns_echo_clientserver" )
add_subdirectory( "und_echo_clientserver" )
endif(WIN32)
