cmake_minimum_required(VERSION 3.15)
project(EngineeredNetworkProject VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Disable updates for fetchcontent" FORCE)

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

message(STATUS "Fetching SDL3_Image from GitHub...")
FetchContent_Declare(
    sdl3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    GIT_TAG        3.4.12
    GIT_SHALLOW    TRUE   # <--- ADD THIS
)

set(SDL_IMAGE_PNG ON CACHE BOOL "Enable PNG support" FORCE)
set(SDL_IMAGE_TESTS OFF CACHE BOOL "Disable SDL_image test suites" FORCE)
set(SDL_IMAGE_SAMPLES OFF CACHE BOOL "Disable SDL_image sample programs" FORCE)
set(SDL_IMAGE_BACKEND_STB ON CACHE BOOL "Use STB backend" FORCE)
set(SDL_IMAGE_PNG_LODEPNG ON CACHE BOOL "Use lodepng backend" FORCE)
FetchContent_MakeAvailable(sdl3_image)

add_library(network_engine 
libs/network/src/network.c
)
target_include_directories(network_engine PUBLIC libs/network/include)

add_executable(my_server 
    src/server.c
)
target_link_libraries(my_server PRIVATE network_engine)

# for game
add_library(physics_engine
    libs/game_engine/src/physics.c
)
add_library(keyboard
    libs/game_engine/src/keyboard.c
)
add_library(rendering
    libs/game_engine/src/rendering.c
)
add_library(arena_alloc 
    libs/game_engine/src/arena_alloc.c
)

target_link_libraries(rendering PRIVATE SDL3::SDL3-shared SDL3_image-shared)
target_link_libraries(keyboard PRIVATE SDL3::SDL3-shared)

target_include_directories(physics_engine PUBLIC libs/game_engine/include)
target_include_directories(keyboard PUBLIC libs/game_engine/include)
target_include_directories(rendering PUBLIC libs/game_engine/include)

add_executable(game
    src/game.c
)
target_include_directories(game PUBLIC libs/game_engine/include)

target_link_libraries(game PRIVATE SDL3::SDL3-shared physics_engine network_engine keyboard rendering m arena_alloc)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")