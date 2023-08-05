
add_library(picprobe)

target_sources(picprobe PRIVATE 
    src/led.c
    src/main.c
    src/usb_descriptors.c
    src/probe.c
    src/cdc_uart.c
    src/get_serial.c
    src/sw_dp_pio.c
)

target_sources(picprobe PRIVATE
    ${CMAKE_SOURCE_DIR}/submodule/CMSIS_5/CMSIS/DAP/Firmware/Source/DAP.c
    ${CMAKE_SOURCE_DIR}/submodule/CMSIS_5/CMSIS/DAP/Firmware/Source/JTAG_DP.c
    ${CMAKE_SOURCE_DIR}/submodule/CMSIS_5/CMSIS/DAP/Firmware/Source/DAP_vendor.c
    ${CMAKE_SOURCE_DIR}/submodule/CMSIS_5/CMSIS/DAP/Firmware/Source/SWO.c
    #CMSIS_5/CMSIS/DAP/Firmware/Source/SW_DP.c
)


target_include_directories(picprobe PRIVATE
    ${CMAKE_SOURCE_DIR}/submodule/CMSIS_5/CMSIS/DAP/Firmware/Include/
    ${CMAKE_SOURCE_DIR}/submodule/CMSIS_5/CMSIS/Core/Include/
    include/
    src/
)


target_compile_options(picprobe PRIVATE -Wall)


pico_generate_pio_header(picprobe ${CMAKE_CURRENT_LIST_DIR}/src/probe.pio)
pico_generate_pio_header(picprobe ${CMAKE_CURRENT_LIST_DIR}/src/probe_oen.pio)


target_compile_definitions (picprobe PRIVATE
	PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
)

target_link_libraries(picprobe PRIVATE
    pico_multicore
    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
    hardware_pio
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap1
)

# pico_set_binary_type(picprobe copy_to_ram)
# pico_add_extra_outputs(picprobe)
