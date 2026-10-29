/*------------------------------------------------------/
/ Copyright (c) 2026, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/------------------------------------------------------*/

// SSD1306 128x64 OLED (I2C) on Raspberry Pi Pico / Pico 2 / RP2040-Zero
//
// The driver is daschr/pico-ssd1306 (lib/pico-ssd1306, left as it is); this
// header adds the default wiring and the C++ guard the driver header lacks.
// Every *_DEFAULT can be overridden with target_compile_definitions().
//
//   GP2  I2C1 SDA  (internal pull-up)
//   GP3  I2C1 SCL  (internal pull-up)

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ssd1306.h"

#ifdef __cplusplus
}
#endif

#ifndef OLED_I2C_INST_DEFAULT
#define OLED_I2C_INST_DEFAULT   i2c1        // GP2 / GP3 are I2C1
#endif
#ifndef OLED_I2C_FREQ_DEFAULT
#define OLED_I2C_FREQ_DEFAULT   (400 * 1000)
#endif
#ifndef PIN_OLED_SDA_DEFAULT
#define PIN_OLED_SDA_DEFAULT    2
#endif
#ifndef PIN_OLED_SCL_DEFAULT
#define PIN_OLED_SCL_DEFAULT    3
#endif
#ifndef OLED_I2C_ADDR_DEFAULT
#define OLED_I2C_ADDR_DEFAULT   0x3C
#endif

#define OLED_WIDTH              128
#define OLED_HEIGHT             64
