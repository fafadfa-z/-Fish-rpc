

cmake_minimum_required(VERSION 3.2)
project(fafadfa_template)

include_directories(Fish)


aux_source_directory(source/base GENERIC_DIR)
aux_source_directory(source/base/log GENERIC_DIR)
aux_source_directory(source/coroutine GENERIC_DIR)
aux_source_directory(source/net GENERIC_DIR)
aux_source_directory(source/rpc GENERIC_DIR)

aux_source_directory(source/registry REGISTRY_DIR)
aux_source_directory(source/provider PROVIDER_DIR)


LINK_DIRECTORIES(/usr/lib)

link_libraries("pthread")

link_libraries(uring)


set(REG_DIR "${GENERIC_DIR};${REGISTRY_DIR}")  
set(PRO_DIR "${GENERIC_DIR};${PROVIDER_DIR}") 


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g -Wall -DDEBUG -fcoroutines")   #debug
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -DDEBUG")   #release

#provider.cpp
add_executable(provider.exe example/provider.cpp ${PRO_DIR})

#registry.cpp
add_executable(registry.exe example/registry.cpp ${REG_DIR})

# #consumer.cpp
# add_executable(registry rpc/registry.cpp acid ${LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)