# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bmp280_i2c C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_subdirectory(inc)
add_executable(bmp280_i2c bmp280_i2c.c inc/bmp280.c)

pico_set_program_name(bmp280_i2c "bmp280_i2c")
pico_set_program_version(bmp280_i2c "0.1")

# Generate PIO header
pico_generate_pio_header(bmp280_i2c ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bmp280_i2c 0)
pico_enable_stdio_usb(bmp280_i2c 1)

# Add the standard library to the build
target_link_libraries(bmp280_i2c
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(bmp280_i2c PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/inc
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add any user requested libraries
target_link_libraries(bmp280_i2c 
        hardware_i2c
        bmp280
        )

pico_add_extra_outputs(bmp280_i2c)

