cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake OPTIONAL RESULT_VARIABLE PICO_SDK_FOUND)

project(hpve_cascade_engine C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(PICO_SDK_FOUND)
    pico_sdk_init()

    add_executable(hpve_cascade_engine
        main.cpp
        pio_cascade_driver.c
        wave_solver.c
        ui_telemetry.c
    )

    pico_generate_pio_header(hpve_cascade_engine ${CMAKE_CURRENT_LIST_DIR}/cascade_spi.pio)

    target_compile_definitions(hpve_cascade_engine PRIVATE
        PICO_BUILD=1
        PICO_RP2040=1
    )

    target_compile_options(hpve_cascade_engine PRIVATE
        -O3
        -Wall
        -Wextra
        -ffast-math
    )

    target_link_libraries(hpve_cascade_engine PRIVATE
        pico_stdlib
        pico_multicore
        hardware_pio
        hardware_dma
        hardware_irq
        hardware_pwm
        hardware_i2c
        hardware_clocks
        hardware_watchdog
        hardware_timer
        hardware_sync
    )

    pico_enable_stdio_usb(hpve_cascade_engine 1)
    pico_enable_stdio_uart(hpve_cascade_engine 0)

    pico_add_extra_outputs(hpve_cascade_engine)
else()
    add_executable(hpve_host_bench
        main.cpp
        pio_cascade_driver.c
        wave_solver.c
        ui_telemetry.c
    )
    target_compile_options(hpve_host_bench PRIVATE -O3 -Wall -Wextra)
    if(UNIX)
        target_link_libraries(hpve_host_bench PRIVATE m)
    endif()
endif()
