cmake_minimum_required(VERSION 3.5.0)
project (warming_iters)

set(CMAKE_CXX_STANDARD 17)

# Windows CYGWIN Linux
message (" \"system name\" ${CMAKE_SYSTEM_NAME}")
# x86_64 AMD64 aarch64
message (" \"processor\" ${CMAKE_HOST_SYSTEM_PROCESSOR}")
# 4 или 8, причем это разрядность компилятора     if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
message (${CMAKE_SIZEOF_VOID_P} . gmock)

cmake_host_system_information(RESULT myvar QUERY HOSTNAME)
# DESKTOP-M4C21IU
message (${myvar})


set(SOURCE_FILES bit_iter.h main.cpp )

add_definitions(-Wno-deprecated )
add_executable(executable ${SOURCE_FILES}   )
