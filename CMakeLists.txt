

cmake_minimum_required(VERSION 3.2)
project(fafadfa_template)

include_directories(Fish)

aux_source_directory(./ DIR_SRCS)
aux_source_directory(source/base DIR_SRCS)
aux_source_directory(source/base/log DIR_SRCS)
aux_source_directory(source/coroutine DIR_SRCS)
aux_source_directory(source/net DIR_SRCS)
aux_source_directory(source/rpc DIR_SRCS)



LINK_DIRECTORIES(/usr/lib)

link_libraries("pthread")

link_libraries(uring)

set(TEST_MATH ${DIR_SRCS})  

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g -Wall -DDEBUG -fcoroutines")   #debug
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -DDEBUG")   #release

add_executable(out.exe ${TEST_MATH})

