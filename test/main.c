#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico_ssd1306_128x64.h"

static ssd1306_t disp;

// 0: as the driver initialises it, 1: turned by 180 deg
// (the driver has no rotation: segment remap and COM scan direction both reversed)
static void oled_set_rotation(uint rotation)
{
    const uint8_t cmds[] = {
        0x00,   // control byte: Co = 0, D/C# = 0 (commands follow)
        (uint8_t) (SET_SEG_REMAP | (rotation ? 0x00 : 0x01)),
        (uint8_t) (SET_COM_OUT_DIR | (rotation ? 0x00 : 0x08)),
    };
    i2c_write_blocking(disp.i2c_i, disp.address, cmds, sizeof(cmds), false);
}

static void draw_circle(int32_t x0, int32_t y0, int32_t r)
{
    int32_t x = r, y = 0, err = 1 - r;
    while (x >= y) {
        ssd1306_draw_pixel(&disp, x0 + x, y0 + y);
        ssd1306_draw_pixel(&disp, x0 + y, y0 + x);
        ssd1306_draw_pixel(&disp, x0 - y, y0 + x);
        ssd1306_draw_pixel(&disp, x0 - x, y0 + y);
        ssd1306_draw_pixel(&disp, x0 - x, y0 - y);
        ssd1306_draw_pixel(&disp, x0 - y, y0 - x);
        ssd1306_draw_pixel(&disp, x0 + y, y0 - x);
        ssd1306_draw_pixel(&disp, x0 + x, y0 - y);
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

// String scrolling to the left, wrapping around with a gap; the caller keeps sft.
// A negative x is passed as uint32_t on purpose: the driver adds the glyph columns
// to it and drops every pixel at x >= width, so the wrapped-around columns are clipped.
static void scroll_string(uint32_t y, const char *s, uint16_t *sft)
{
    const int32_t w = (int32_t) strlen(s) * 6;    // 5 px glyph + 1 px spacing
    const int32_t period = w + OLED_WIDTH / 2;
    ssd1306_clear_square(&disp, 0, y, OLED_WIDTH, 8);
    for (int32_t x = -(int32_t) *sft; x < OLED_WIDTH; x += period) {
        ssd1306_draw_string(&disp, (uint32_t) x, y, 1, s);
    }
    *sft = (*sft + 1) % period;
}

void testtexts()
{
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 31, 0*8, 1, "Hello World");
    ssd1306_draw_string(&disp, 31, 1*8, 1, "Hello World");
    ssd1306_draw_string(&disp, 35, 2*8, 2, "Hello");
    ssd1306_draw_string(&disp, 38, 5*8, 3, "SSD");
    ssd1306_show(&disp);
}

void testpoints()
{
    ssd1306_clear(&disp);
    for (int32_t y=4; y < OLED_HEIGHT; y+=8) {
        for (int32_t x=4; x < OLED_WIDTH; x+=8) {
            if (x == 4 || y == 4 || x >= OLED_WIDTH-8 || y >= OLED_HEIGHT-8) {
                ssd1306_draw_square(&disp, x-1, y-1, 3, 3);
            } else {
                ssd1306_draw_pixel(&disp, x, y);
            }
        }
    }
    ssd1306_show(&disp);
}

static void testlines_from(int32_t x0, int32_t y0)
{
    const int32_t x1 = OLED_WIDTH-1 - x0;
    const int32_t y1 = OLED_HEIGHT-1 - y0;
    ssd1306_clear(&disp);
    for (int32_t x=0; x < OLED_WIDTH; x+=6) {
        ssd1306_draw_line(&disp, x0, y0, x, y1);
        ssd1306_show(&disp);
    }
    for (int32_t y=0; y < OLED_HEIGHT; y+=6) {
        ssd1306_draw_line(&disp, x0, y0, x1, y);
        ssd1306_show(&disp);
    }
}

void testlines()
{
    testlines_from(0, 0);
    testlines_from(OLED_WIDTH-1, 0);
    testlines_from(0, OLED_HEIGHT-1);
    testlines_from(OLED_WIDTH-1, OLED_HEIGHT-1);
}

void testdrawrects()
{
    ssd1306_clear(&disp);
    for (int32_t x=0; x < OLED_WIDTH; x+=6) {
        const int32_t h = x*OLED_HEIGHT/OLED_WIDTH;
        ssd1306_draw_empty_square(&disp, OLED_WIDTH/2 -x/2, OLED_HEIGHT/2 -h/2, x, h);
    }
    ssd1306_show(&disp);
}

void testfillrects()
{
    ssd1306_clear(&disp);
    bool fill = true;
    for (int32_t x=OLED_WIDTH-1; x > 8; x-=8) {
        const int32_t h = x*OLED_HEIGHT/OLED_WIDTH;
        if (fill) {
            ssd1306_draw_square(&disp, OLED_WIDTH/2 -x/2, OLED_HEIGHT/2 -h/2, x, h);
        } else {
            ssd1306_clear_square(&disp, OLED_WIDTH/2 -x/2, OLED_HEIGHT/2 -h/2, x, h);
        }
        fill = !fill;
    }
    ssd1306_show(&disp);
}

void testdrawcircles(int32_t radius)
{
    ssd1306_clear(&disp);
    for (int32_t x=radius; x < OLED_WIDTH; x+=radius*2) {
        for (int32_t y=radius; y < OLED_HEIGHT; y+=radius*2) {
            for (int32_t r = radius; r > 0; r-=2) {
                draw_circle(x, y, r);
            }
        }
    }
    ssd1306_show(&disp);
}

void testinvert(uint32_t time_stay)
{
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 29, 24, 2, "Invert");
    ssd1306_draw_empty_square(&disp, 0, 0, OLED_WIDTH-1, OLED_HEIGHT-1);
    ssd1306_show(&disp);
    for (int i = 0; i < 4; i++) {
        ssd1306_invert(&disp, (i & 1) ? 0 : 1);
        sleep_ms(time_stay / 2);
    }
    ssd1306_invert(&disp, 0);
}

void testscrolltexts(uint rotation)
{
    char str[24];
    uint16_t sft0 = 0;
    uint16_t sft1 = 0;

    ssd1306_clear(&disp);
    snprintf(str, sizeof(str), "Rot: %3d", rotation * 180);
    ssd1306_draw_string(&disp, 0, 5*8, 1, str);
    for (int i = 0; i < 512; i++) {
        scroll_string(0*8, "Raspberry Pi Pico is a tiny, fast, and versatile board built using RP2040.", &sft0);
        if (i & 1) {
            scroll_string(2*8, "RP2040 features a dual-core Arm Cortex-M0+ processor with 264KB internal RAM and support for up to 16MB of off-chip Flash.", &sft1);
        }
        snprintf(str, sizeof(str), "Frame: %3d", i);
        ssd1306_clear_square(&disp, 0, 7*8, OLED_WIDTH, 8);
        ssd1306_draw_string(&disp, 0, 7*8, 1, str);
        ssd1306_show(&disp);
    }
}

int main()
{
    const uint32_t TimeStay = 1000;
    uint rotation = 0;

    stdio_init_all();

    i2c_init(OLED_I2C_INST_DEFAULT, OLED_I2C_FREQ_DEFAULT);
    gpio_set_function(PIN_OLED_SDA_DEFAULT, GPIO_FUNC_I2C);
    gpio_set_function(PIN_OLED_SCL_DEFAULT, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_OLED_SDA_DEFAULT);
    gpio_pull_up(PIN_OLED_SCL_DEFAULT);

    printf("\n");
    printf("=========================\n");
    printf("== pico_ssd1306_128x64 ==\n");
    printf("=========================\n");
    printf("SSD1306 %dx%d at 0x%02X (SDA=GP%d SCL=GP%d)\n",
           OLED_WIDTH, OLED_HEIGHT, OLED_I2C_ADDR_DEFAULT, PIN_OLED_SDA_DEFAULT, PIN_OLED_SCL_DEFAULT);

    // the module's reset is an on-board RC: give the panel a moment after power-on
    sleep_ms(100);
    disp.external_vcc = false;
    if (!ssd1306_init(&disp, OLED_WIDTH, OLED_HEIGHT, OLED_I2C_ADDR_DEFAULT, OLED_I2C_INST_DEFAULT)) {
        printf("ssd1306_init failed\n");
        while (true) {
            tight_loop_contents();
        }
    }

    while (1) {
        printf("Rotation: %d deg\n", rotation * 180);
        oled_set_rotation(rotation);

        testtexts();
        sleep_ms(TimeStay);

        testpoints();
        sleep_ms(TimeStay);

        testlines();
        sleep_ms(TimeStay);

        testdrawrects();
        sleep_ms(TimeStay);

        testfillrects();
        sleep_ms(TimeStay);

        testdrawcircles(10);
        sleep_ms(TimeStay);

        testinvert(TimeStay);

        // Demo end (Text Scroll)
        testscrolltexts(rotation);

        rotation ^= 1;
    }
    printf("\nDone\n");

    return 0;
}
