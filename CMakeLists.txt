cmake_minimum_required(VERSION 3.15)
project(EngineeredNetworkProject VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(network_engine 
libs/network/src/network.c
)
target_include_directories(network_engine PUBLIC libs/network/include)

add_executable(my_server 
    src/server.c
)
target_link_libraries(my_server PRIVATE network_engine)

add_executable(my_client 
    src/client.c
)
target_link_libraries(my_client PRIVATE network_engine)