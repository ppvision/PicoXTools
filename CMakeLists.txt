cmake_minimum_required(VERSION 3.13)

option(USB_CONSOLE "build for USB console, otherwise UART" OFF)
option(FORCE_TESTS "build release with tests cmd" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
endif ()


if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR "PICO SDK location was not specified. Please set PICO_SDK_PATH ")
endif()

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)


set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/submodule/freertos)

if (NOT EXISTS ${FREERTOS_KERNEL_PATH})
    message(FATAL_ERROR "Directory '${FREERTOS_KERNEL_PATH}' not found")
endif()


include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# FreeRTOS-Kernel-Core
# FreeRTOS-Kernel
# FreeRTOS-Kernel-Heap1
# FreeRTOS-Kernel-Heap2
# FreeRTOS-Kernel-Heap3
# FreeRTOS-Kernel-Heap4
# FreeRTOS-Kernel-Heap5


if (USB_CONSOLE)
  set(PSHELL PicoXTools_usb)
else()
  set(PSHELL PicoXTools_uart)
endif()

project(${PSHELL} C CXX ASM)

pico_sdk_init()

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.0 (or later) required.")
endif()

find_package(Git REQUIRED)
execute_process(
  COMMAND git describe
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE PSHELL_GIT_TAG
)

add_subdirectory(submodule/littlefs)
add_subdirectory(submodule/disassembler)

add_subdirectory(components/st7735_80x160)
add_subdirectory(components/sx126x_driver)
add_subdirectory(components/lr11xx_driver)

add_subdirectory(apps/picprobe)



if ("${PICO_BOARD}" STREQUAL "vgaboard")
  add_subdirectory(components/sdio)
else()
  add_subdirectory(components/flashio)
endif()

add_executable(${PSHELL}
    src/main.c
    src/utils/crc16.c 
    src/bsp/tty_uart.c 
    src/readline/readln.c
        src/shell/cmds/xmodem.c
        src/shell/cmds/ymodem.c
        src/shell/shell.c
        src/shell/fs_cmd.c
        src/shell/cmds/vi.c
        src/shell/cmds/cc/cc.c
        src/shell/cmds/cc/cc_malloc.c
        src/shell/cmds/cc/cc_printf.S
        src/shell/cmds/tar.c
    src/common/ringbuffer.c
    src/common/tty.c
    src/tests.c
    src/rtos_hook.c
)
#  PICO_FLASH_SIZE_BYTES=16*1024*1024
target_compile_definitions(${PSHELL} PUBLIC
  PICO_MALLOC_PANIC=0
  PSHELL_GIT_TAG=\"${PSHELL_GIT_TAG}\"
  PICO_FLASH_SIZE_BYTES=16*1024*1024
)

if (FORCE_TESTS)
    target_compile_definitions(${PSHELL} PUBLIC PSHELL_TESTS)
endif()

target_include_directories(${PSHELL} PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/config/
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/src/bsp
    ${CMAKE_CURRENT_LIST_DIR}/src/common
    ${CMAKE_CURRENT_LIST_DIR}/src/utils
    ${CMAKE_CURRENT_LIST_DIR}/src/oled
    ${CMAKE_CURRENT_LIST_DIR}/src/readline
    ${CMAKE_CURRENT_LIST_DIR}/src/shell
    ${CMAKE_CURRENT_LIST_DIR}/src/shell/cmds
    ${CMAKE_CURRENT_LIST_DIR}/src/shell/cmds/cc
    ${CMAKE_CURRENT_LIST_DIR}/src/${FS_DIR}
)

pico_set_linker_script(${PSHELL} ${CMAKE_CURRENT_LIST_DIR}/src/pshell.ld)

if (USB_CONSOLE)
    pico_enable_stdio_uart(${PSHELL} 0)
    pico_enable_stdio_usb(${PSHELL} 1)
else()
    pico_enable_stdio_uart(${PSHELL} 1)
    pico_enable_stdio_usb(${PSHELL} 0)
endif()

target_link_libraries(${PSHELL} PUBLIC
    littlefs disassembler io st7735_80x160
    pico_stdlib pico_rand
    hardware_flash hardware_sync hardware_watchdog hardware_timer hardware_gpio
    hardware_pwm hardware_adc hardware_clocks hardware_uart hardware_i2c
    hardware_spi hardware_irq hardware_dma  picprobe
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap1
    sx126x_driver
    lr11xx_driver

)

pico_add_extra_outputs(${PSHELL})

message("-----------------------------------------------------")
message("-- CMake ${CMAKE_VERSION}")
message("-- Build ${CMAKE_BUILD_TYPE} / ${CMAKE_SYSTEM_NAME}")
if (USB_CONSOLE)
message("-- USB_CONSOLE ${USB_CONSOLE}, using USB")
else()
message("-- USB_CONSOLE ${USB_CONSOLE}, using UART")
endif()
if ("${PICO_BOARD}" STREQUAL "vgaboard")
message("-- building for ${PICO_BOARD}, using SD card file system")
else()
message("-- building for ${PICO_BOARD}, using flash file system")
endif()
message("-----------------------------------------------------")

