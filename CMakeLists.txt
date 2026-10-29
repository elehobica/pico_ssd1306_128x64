#------------------------------------------------------
# Copyright (c) 2026, Elehobica
# Released under the BSD-2-Clause
# refer to https://opensource.org/licenses/BSD-2-Clause
#------------------------------------------------------

# SSD1306 128x64 OLED (I2C) library: daschr/pico-ssd1306 (submodule at
# lib/pico-ssd1306) plus pico_ssd1306_128x64.h (default pins, C++ guard)
if (NOT TARGET pico_ssd1306_128x64)
    add_library(pico_ssd1306_128x64 INTERFACE)

    target_sources(pico_ssd1306_128x64 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/lib/pico-ssd1306/ssd1306.c
    )

    target_link_libraries(pico_ssd1306_128x64 INTERFACE
        hardware_i2c
        pico_stdlib
    )
    target_include_directories(pico_ssd1306_128x64 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib/pico-ssd1306
    )
endif()
