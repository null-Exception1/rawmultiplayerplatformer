cmake_minimum_required(VERSION 3.15)
project(EngineeredNetworkProject VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
message(STATUS "Fetching SDL3 from GitHub...")
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG        main 
)

set(SDL_SHARED ON CACHE BOOL "Build shared SDL library")
set(SDL_STATIC OFF CACHE BOOL "Disable static SDL library")
set(SDL_TEST OFF CACHE BOOL "Disable SDL test suites")

FetchContent_MakeAvailable(SDL3)

add_library(network_engine 
libs/network/src/network.c
)
target_include_directories(network_engine PUBLIC libs/network/include)

add_executable(my_server 
    src/server.c
)
target_link_libraries(my_server PRIVATE network_engine)

add_executable(game
    src/game.c
)
target_link_libraries(game PRIVATE SDL3::SDL3)
target_link_libraries(game PRIVATE network_engine)