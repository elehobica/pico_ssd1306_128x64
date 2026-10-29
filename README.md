# SSD1306 128x64 OLED Library for Raspberry Pi Pico / Pico 2

[![Build](https://github.com/elehobica/pico_ssd1306_128x64/actions/workflows/build-binaries.yml/badge.svg)](https://github.com/elehobica/pico_ssd1306_128x64/actions/workflows/build-binaries.yml)

## Overview
SSD1306 128x64 OLED (I2C) display library and demo for Raspberry Pi Pico / Pico 2

This project supports:
* SSD1306 128x64 OLED module (4-pin I2C type)
* Drawing by [daschr/pico-ssd1306](https://github.com/daschr/pico-ssd1306) (included as a git submodule)

## Supported Board
* Raspberry Pi Pico
* Raspberry Pi Pico 2
* Waveshare RP2040-Zero
* SSD1306 128x64 OLED module (I2C)

## Pin Assignment
### SSD1306 128x64 OLED

| Pin Name | Function | SSD1306 OLED Module |
----|----|----
| GP2 | I2C1_SDA | SDA |
| GP3 | I2C1_SCL | SCL |
| GND | GND | GND |
| 3V3(OUT) | 3.3V | VCC |

* SDA / SCL use the internal pull-ups (most modules also have their own pull-up resistors)
* I2C address 0x3C, 400 kHz

### Serial (CP2102 module)
| Pin Name | Function | CP2102 module |
----|----|----
| GP0 | UART0_TX | RXD |
| GP1 | UART0_RX | TXD |
| GND | GND | GND |

## How to get the source
The driver is a git submodule, so clone with `--recursive` (or run `git submodule update --init` after cloning)
```
$ git clone --recursive -b main https://github.com/elehobica/pico_ssd1306_128x64.git
```

## How to build with docker image
* Builds the firmware inside [pico-sdk-dev-docker:sdk-2.3.0](https://hub.docker.com/r/elehobica/pico-sdk-dev-docker) (same image used by CI). Requires Docker; no local Pico SDK setup is needed.
* `build_docker.sh` drives the container build. The project to build is taken from the current directory, so run it from inside the project folder you want to build (`test`).
```
$ cd pico_ssd1306_128x64/test
$ ../build_docker.sh           # build all targets (default)
$ ../build_docker.sh pico      # build only Pico         -> build/pico_ssd1306_128x64_test.uf2
$ ../build_docker.sh pico2     # build only Pico 2       -> build2/pico_ssd1306_128x64_test.uf2
$ ../build_docker.sh zero      # build only RP2040-Zero  -> build_zero/pico_ssd1306_128x64_test.uf2
```
* Download "*.uf2" on RPI-RP2 or RP2350 drive

## How to build in local
* See ["Getting started with Raspberry Pi Pico"](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
* Put "pico-sdk" on the same level with this project folder.
* Set environmental variable for PICO_SDK_PATH
* Confirmed with Pico SDK 2.3.0
```
> git clone -b 2.3.0 https://github.com/raspberrypi/pico-sdk.git
> cd pico-sdk
> git submodule update -i
> cd ..
> git clone --recursive -b main https://github.com/elehobica/pico_ssd1306_128x64.git
```
### Windows
* Lanuch "Developer Command Prompt for VS 2022"
```
> cd pico_ssd1306_128x64\test
> mkdir build && cd build
> cmake -G "NMake Makefiles" ..  ; (for Raspberry Pi Pico 1 series)
> cmake -G "NMake Makefiles" -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..  ; (for Raspberry Pi Pico 2)
> cmake -G "NMake Makefiles" -DPICO_BOARD=waveshare_rp2040_zero ..  ; (for Waveshare RP2040-Zero)
> nmake
```
* Put "*.uf2" on RPI-RP2 or RP2350 drive
### Linux
* Build is confirmed with [pico-sdk-dev-docker:sdk-2.3.0]( https://hub.docker.com/r/elehobica/pico-sdk-dev-docker)
```
$ cd pico_ssd1306_128x64/test
$ mkdir build && cd build
$ cmake ..  # (for Raspberry Pi Pico 1 series)
$ cmake -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..  # (for Raspberry Pi Pico 2)
$ cmake -DPICO_BOARD=waveshare_rp2040_zero ..  # (for Waveshare RP2040-Zero)
$ make -j4
```
* Download "*.uf2" on RPI-RP2 or RP2350 drive

## How to use from another project
```
add_subdirectory(<path to pico_ssd1306_128x64> pico_ssd1306_128x64)
target_link_libraries(<your target> pico_ssd1306_128x64)
```
* Include `pico_ssd1306_128x64.h` (it includes the driver's `ssd1306.h` with a C++ guard)
* Default wiring macros (`OLED_I2C_INST_DEFAULT`, `OLED_I2C_FREQ_DEFAULT`, `PIN_OLED_SDA_DEFAULT`, `PIN_OLED_SCL_DEFAULT`, `OLED_I2C_ADDR_DEFAULT`) can be overridden by `target_compile_definitions()`
* See [test/main.c](test/main.c) for the I2C setup and `ssd1306_init()`
