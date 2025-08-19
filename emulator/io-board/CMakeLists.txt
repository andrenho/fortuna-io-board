cmake_minimum_required(VERSION 3.13)
project(fiob-emulator-exe ASM C)
set(CMAKE_C_STANDARD 23)

add_executable(fiob-emulator-demo
        demo.c
)

target_link_libraries(fiob-emulator-demo PRIVATE raylib fiob-lib)