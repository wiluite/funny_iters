cmake_minimum_required(VERSION 3.5.0)
project (funny_iters)

set(CMAKE_CXX_STANDARD 17)

set (BOOST_TEST_COMPONENTS  unit_test_framework )
find_package (Boost REQUIRED COMPONENTS ${BOOST_TEST_COMPONENTS} )
include_directories(${Boost_INCLUDE_DIRS})
if (UNIX)
    add_definitions( -DBOOST_TEST_DYN_LINK )
endif()


# Windows CYGWIN Linux
message (" \"system name\" ${CMAKE_SYSTEM_NAME}")
# x86_64 AMD64 aarch64
message (" \"processor\" ${CMAKE_HOST_SYSTEM_PROCESSOR}")
# 4 or 8 bits of pointer size
message (${CMAKE_SIZEOF_VOID_P})

cmake_host_system_information(RESULT myvar QUERY HOSTNAME)
# DESKTOP-M4C21IU
message (${myvar})

set(SOURCE_FILES bit_iter.h main.cpp ring_iter.h)

add_definitions(-Wno-deprecated )
add_executable(executable ${SOURCE_FILES}   )


add_executable(test_app test.cpp)
target_link_libraries (test_app ${Boost_LIBRARIES}  )
add_test (test_app test_app)
