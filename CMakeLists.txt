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
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Assignment_2 C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Assignment_2 src/main.c src/asm.S src/morse_utils.c
                            src/display_utils.c src/game_logic.c src/sdk_asm_helpers.c
                            src/asm_interface.c src/ws2812.c)

pico_set_program_name(Assignment_2 "Assignment_2")
pico_set_program_version(Assignment_2 "0.1")
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Assignment_2 1)
pico_enable_stdio_usb(Assignment_2 1)

# Add the standard library to the build
target_link_libraries(Assignment_2
        pico_stdlib
        hardware_pio
        hardware_timer
        hardware_watchdog
        hardware_clocks)

# Add the standard include files to the build
target_include_directories(Assignment_2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Assignment_2)

