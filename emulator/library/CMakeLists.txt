cmake_minimum_required(VERSION 3.13)
project(fiob-lib ASM C)
set(CMAKE_C_STANDARD 23)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

add_library(${PROJECT_NAME} STATIC
        fortuna.h           fortuna.c
        vga/framebuffer.h   vga/framebuffer.c
        vga/vga.h           vga/vga.c
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${raylib_SOURCE_DIR}/src")