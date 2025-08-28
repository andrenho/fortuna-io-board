cmake_minimum_required(VERSION 3.13)
project(fiob-emulator-exe ASM C)
set(CMAKE_C_STANDARD 23)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

add_executable(fiob-emulator-demo
        main.c
        demo/demo.c
        terminal/terminal.c
)

target_link_libraries(fiob-emulator-demo PRIVATE fiob-lib SDL3::SDL3)