cmake_minimum_required(VERSION 3.13)
project(fiob-lib ASM C)
set(CMAKE_C_STANDARD 23)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

add_library(${PROJECT_NAME} STATIC
        fortuna.h               fortuna.c
        vga/framebuffer.h       vga/framebuffer.c
        vga/vga.h               vga/vga.c
        sdcard/ff/ff.h          sdcard/ff/ff.c
        sdcard/ff/f_util.h      sdcard/ff/f_util.c
        sdcard/ff/diskio.h      sdcard/diskio.c
        sdcard/ff/ffunicode.c
        usb/sdlhid.h            usb/sdlhid.c
        panel/panel.h           panel/panel.c
        rtc/rtc.h               rtc/rtc.c
        audio/audio.h           audio/audio.c
        config/config.h         config/config.c
        terminal/terminal.h     terminal/terminal.c
        contrib/libtmt/tmt.h    contrib/libtmt/tmt.c
        pico/pico.h             pico/pico.c
        pico/time.h             pico/time.c
        pico/util/queue.h       pico/util/queue.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/sdcard"
        "${CMAKE_CURRENT_SOURCE_DIR}/sdcard/ff"
        "${raylib_SOURCE_DIR}/src"
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)