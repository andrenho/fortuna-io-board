cmake_minimum_required(VERSION 3.13)

include(../pico_sdk_import.cmake)

project(fortuna-io-board-demo ASM C CXX)

set(PICO_BOARD pico2 CACHE STRING "Board type")

pico_sdk_init()

add_subdirectory(../library build)

add_compile_options(-O3)

add_executable(${CMAKE_PROJECT_NAME}
        main.c
        demo/demo.c
        terminal/terminal.c)

target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)

target_link_libraries(${CMAKE_PROJECT_NAME}
    fortuna-io-board)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

# upload remotely - see https://github.com/andrenho/remoteprog
add_custom_target(upload-remote
        COMMAND remoteprog upload -c pico2 ${CMAKE_PROJECT_NAME}.elf
        DEPENDS ${CMAKE_PROJECT_NAME}
        COMMENT "Uploading remotely..."
        VERBATIM
)
