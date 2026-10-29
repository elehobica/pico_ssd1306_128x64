# Raspberry Pi Pico SSD1306 128x64 OLED Library Demo test

## Pin Assignment
In addition to original connection

### Serial (CP2102 module)
| Pin Name | Function | CP2102 module |
----|----|----
| GP0 | UART0_TX | RXD |
| GP1 | UART0_RX | TXD |
| GND | GND | GND |

## Demo sequence
The demo starts right after power-on and repeats the following, turning the picture by 180 deg on every round
* Texts (scale 1, 2, 3)
* Points
* Lines
* Rectangles
* Filled rectangles
* Circles
* Display inversion
* Scrolling texts
