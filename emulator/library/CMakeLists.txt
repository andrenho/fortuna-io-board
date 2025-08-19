cmake_minimum_required(VERSION 3.13)
project(fiob-lib ASM C)
set(CMAKE_C_STANDARD 23)

add_library(${PROJECT_NAME} STATIC
        fortuna.h
        fortuna.c
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")