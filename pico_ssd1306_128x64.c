/*------------------------------------------------------/
/ Copyright (c) 2026, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/------------------------------------------------------*/

#include "pico_ssd1306_128x64.h"

#include <string.h>

static const uint32_t OLED_I2C_TIMEOUT_US = 5000;   // never hang on a stuck bus (hot plug)

// a command sequence in one transaction: control byte 0x00 (Co = 0, D/C# = 0), then the commands
static bool oled_cmds(ssd1306_t *p, const uint8_t *cmds, size_t n)
{
    uint8_t buf[8];
    if (n + 1 > sizeof(buf)) return false;
    buf[0] = 0x00;
    memcpy(buf + 1, cmds, n);
    return i2c_write_timeout_us(p->i2c_i, p->address, buf, n + 1, false, OLED_I2C_TIMEOUT_US) == (int) (n + 1);
}

// page n, column col (both controllers in page addressing)
static bool oled_set_page_col(ssd1306_t *p, uint8_t page, uint8_t col)
{
    const uint8_t cmds[] = { (uint8_t) (0xB0 | page), (uint8_t) (0x00 | (col & 0x0F)), (uint8_t) (0x10 | (col >> 4)) };
    return oled_cmds(p, cmds, sizeof(cmds));
}

bool oled_set_page_addressing(ssd1306_t *p)
{
    // SSD1306: page addressing mode. SH1106 has page addressing only: 0x20 is no command
    // there and 0x02 sets the lower column nibble, which oled_show() sets again anyway
    const uint8_t cmds[] = { SET_MEM_ADDR, 0x02 };
    return oled_cmds(p, cmds, sizeof(cmds));
}

bool oled_read_status(ssd1306_t *p, uint8_t *status)
{
    const uint8_t ctrl = 0x00;  // D/C# = 0: status
    if (i2c_write_timeout_us(p->i2c_i, p->address, &ctrl, 1, true, OLED_I2C_TIMEOUT_US) != 1) return false;
    return i2c_read_timeout_us(p->i2c_i, p->address, status, 1, false, OLED_I2C_TIMEOUT_US) == 1;
}

// Write a pattern to page 0 and read it back with D/C# = 1. Only SH1106 returns
// display data over I2C (after one dummy byte); SSD1306 returns its status byte
// for every read. A module that does not wire the controller's data output to
// SDA reads 0xFF (pull-up): undecidable. Page 0 is overwritten, so draw after this.
bool oled_detect(ssd1306_t *p, oled_controller_t *ctrl, uint8_t readback[OLED_DETECT_READ_BYTES])
{
    static const uint8_t pattern[] = { 0xA5, 0x5A, 0x3C };
    uint8_t wr[1 + sizeof(pattern)] = { 0x40 };   // control byte: D/C# = 1 (data)
    memcpy(wr + 1, pattern, sizeof(pattern));
    memset(readback, 0xFF, OLED_DETECT_READ_BYTES);

    if (!oled_set_page_addressing(p)) return false;
    if (!oled_set_page_col(p, 0, 0)) return false;
    if (i2c_write_timeout_us(p->i2c_i, p->address, wr, sizeof(wr), false, OLED_I2C_TIMEOUT_US) != (int) sizeof(wr)) return false;
    if (!oled_set_page_col(p, 0, 0)) return false;
    if (i2c_write_timeout_us(p->i2c_i, p->address, wr, 1, true, OLED_I2C_TIMEOUT_US) != 1) return false;
    if (i2c_read_timeout_us(p->i2c_i, p->address, readback, OLED_DETECT_READ_BYTES, false, OLED_I2C_TIMEOUT_US) != OLED_DETECT_READ_BYTES) return false;

    bool all_ff = true;
    for (int i = 0; i < OLED_DETECT_READ_BYTES; i++) {
        if (readback[i] != 0xFF) all_ff = false;
    }
    if (all_ff) return false;
    // the pattern right after the dummy byte (or without it, to be lenient)
    if (memcmp(readback + 1, pattern, sizeof(pattern)) == 0 || memcmp(readback, pattern, sizeof(pattern)) == 0) {
        *ctrl = OLED_SH1106;
    } else {
        *ctrl = OLED_SSD1306;
    }
    return true;
}

// Replacement for ssd1306_show(): page by page in page addressing, which both
// controllers have (ssd1306_show() relies on SSD1306's horizontal addressing and
// its column / page range commands: on SH1106 the whole frame lands in page 0).
// SH1106 has 132 columns of RAM with the 128 px panel in the middle.
void oled_show(ssd1306_t *p, oled_controller_t ctrl)
{
    const uint8_t col_ofs = (ctrl == OLED_SH1106) ? SH1106_COL_OFS : 0;
    uint8_t buf[1 + OLED_WIDTH];
    buf[0] = 0x40;  // control byte: D/C# = 1 (data)
    for (uint8_t page = 0; page < p->pages; page++) {
        if (!oled_set_page_col(p, page, col_ofs)) return;
        memcpy(buf + 1, p->buffer + page * p->width, p->width);
        if (i2c_write_timeout_us(p->i2c_i, p->address, buf, p->width + 1, false, OLED_I2C_TIMEOUT_US) != p->width + 1) return;
    }
}
