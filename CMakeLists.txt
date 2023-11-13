# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
#include(pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(UART_MODBUS C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.c
    uart_lib/uart.c
    modbus_lib/modbus.c
    #core_1/core_1_process.c
)


# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_ (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib
    hardware_uart
    pico_multicore
    pico_printf
    #hardware_i2c
    #hardware_timer
    #hardware_pwm
    #hardware_irq
    
    #"${CMAKE_CURRENT_SOURCE_DIR}/rp2040_nodeConstants.h"
)

add_library(uart_lib
    uart_lib/uart.c
    uart_lib/uart.h
)

add_library(modbus_lib
    modbus_lib/modbus.c
    modbus_lib/modbus.h
)
#add_library(core_1 core_1/core_1_process.c core_1/core_1_process.h)

target_link_libraries(uart_lib
    pico_stdlib
    hardware_uart
    pico_printf
)

target_link_libraries(modbus_lib
    pico_stdlib
    pico_printf
)

#target_link_libraries(core_1
#    pico_multicore
#)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
# Messages should appear on both with printf()