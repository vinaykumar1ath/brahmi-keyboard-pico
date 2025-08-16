cmake_minimum_required(VERSION 3.13)
#set(PICO_SDK_PATH "C:\Program Files\Raspberry Pi\Pico SDK v1.5.1\pico-sdk" )
#set (PICO_TOOLCHAIN_PATH "C:\Program Files\Raspberry Pi\Pico SDK v1.5.1\gcc-arm-none-eabi" )
include (../pico_device_and_host/pico_sdk_import.cmake)
project(BRAHMI_KEYBOARD C CXX ASM)

pico_sdk_init()

add_subdirectory("../pico_device_and_host/Pico-PIO-USB" pico_pio_usb)

set(target_name BRAHMI_KEYBOARD)
add_executable(${target_name})
target_sources(${target_name} PRIVATE
 ./src/modified_main_device.c
 ./src/modified_main_host.c
 ../pico_device_and_host/usb_descriptors.c
 ./src/lib.c
 ./src/config.c
 ./src/utils.c
 ./src/generated_layouts.c
 # can use 'tinyusb_pico_pio_usb' library later when pico-sdk is updated
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
 )




 # print memory usage, enable all warnings
target_link_options(${target_name} PRIVATE -Xlinker --print-memory-usage)
target_compile_options(${target_name} PRIVATE ) #-Wall -Wextra

# use tinyusb implementation
target_compile_definitions(${target_name} PRIVATE PIO_USB_USE_TINYUSB)

# needed so tinyusb can find tusb_config.h
target_include_directories(${target_name} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}
    ../pico_device_and_host
)

target_link_libraries(${target_name} PRIVATE pico_stdlib pico_pio_usb tinyusb_device tinyusb_host)
pico_add_extra_outputs(${target_name})

