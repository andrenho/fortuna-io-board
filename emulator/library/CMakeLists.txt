cmake_minimum_required(VERSION 3.13)
project(fiob-lib ASM C)
set(CMAKE_C_STANDARD 23)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

add_library(${PROJECT_NAME} STATIC
        fortuna.h            fortuna.c
        vga/framebuffer.h    vga/framebuffer.c
        vga/vga.h            vga/vga.c
        terminal/terminal.h  terminal/terminal.c
        contrib/libtmt/tmt.h contrib/libtmt/tmt.c
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${raylib_SOURCE_DIR}/src")
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)