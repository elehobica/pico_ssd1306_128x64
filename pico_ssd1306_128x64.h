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

// Modules that look the same carry SSD1306 (or the compatible SSD1315) or SH1106
typedef enum {
    OLED_SSD1306 = 0,
    OLED_SH1106,
} oled_controller_t;

// used when oled_detect() cannot tell
#ifndef OLED_CONTROLLER_DEFAULT
#define OLED_CONTROLLER_DEFAULT OLED_SSD1306
#endif
// SH1106: 132 columns of RAM, the 128 px panel starts at this column
#ifndef SH1106_COL_OFS
#define SH1106_COL_OFS          2
#endif

#define OLED_DETECT_READ_BYTES  4

#ifdef __cplusplus
extern "C" {
#endif

// call after ssd1306_init(): page addressing, which oled_show() needs
bool oled_set_page_addressing(ssd1306_t *p);
// status byte (D/C# = 0 read); false if the read fails
bool oled_read_status(ssd1306_t *p, uint8_t *status);
// SSD1306 / SH1106 by display RAM read-back; false if undecidable.
// readback: the raw bytes read (for logging). Overwrites page 0 of the panel.
bool oled_detect(ssd1306_t *p, oled_controller_t *ctrl, uint8_t readback[OLED_DETECT_READ_BYTES]);
// use instead of ssd1306_show(): works on both controllers
void oled_show(ssd1306_t *p, oled_controller_t ctrl);

#ifdef __cplusplus
}
#endif
