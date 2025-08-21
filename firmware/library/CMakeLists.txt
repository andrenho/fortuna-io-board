cmake_minimum_required(VERSION 3.13)
string(TIMESTAMP YEAR "%Y")

#
# configuration
#

set(PICO_BOARD pico2 CACHE STRING "Board type" PARENT_SCOPE)

set(CMAKE_C_STANDARD 23 PARENT_SCOPE)
set(CMAKE_C_STANDARD_REQUIRED ON PARENT_SCOPE)

include(../pico_sdk_import.cmake)
pico_sdk_init()

#
# project
#

project(fortuna-io-board VERSION 2.0)

add_library(${PROJECT_NAME} INTERFACE)

#
# sources
#

target_sources(${PROJECT_NAME} INTERFACE
		fortuna.h			fortuna.c
		vga/color.h
		vga/font.h			vga/fortuna_font.h
		vga/vga.h 			vga/vga.c
		vga/framebuffer.h  	vga/framebuffer.c
		usb/tusb_config.h
)

#add_subdirectory(contrib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/hsync.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/vsync.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/rgb640.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga/pio/rgb320.pio)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/usb")
target_compile_definitions(${PROJECT_NAME} INTERFACE -DPROJECT_VERSION=\"${PROJECT_VERSION}\" -DYEAR=\"${YEAR}\" -DFIRMWARE=1)

pico_enable_stdio_uart(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME} INTERFACE
	pico_stdlib 
	pico_bootsel_via_double_reset
	hardware_pio 
	hardware_dma
	hardware_irq
	hardware_i2c
	hardware_pwm
	pico_aon_timer
	pico_multicore
    tinyusb_host
    tinyusb_board
    #no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)
