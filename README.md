# LedStripController
A PlatformIO firmware project providing LED Strip effects via an ASCII protocol
over serial interface.


# Hardware Requirements
LedStripController should run on most Arduino hardware but has primarily been 
tested on Teensy 3.2 devices: https://www.pjrc.com/store/teensy32.html


# Build
The following has been tested on Ubuntu Linux but should be adaptable on other
Linux distributions. It is also assumed Rust language tools have been installed.

1. Install platformIO Core
    Note: You can Use the PlatformIO IDE via VSCode but only core is required.
    
    https://docs.platformio.org/en/latest//core/installation.html
    
1. Build

    `pio run`
    
1. Build & Upload to Teensy

    `pio run --target upload`


# Control via Terminal
Control via the terminal is only feasible is the firmware is built with CRC16 
checks disabled. Responses will include CRC16 suffix but will not validate 
control command CRC's. In order to disable CRC checks set `DISABLE_CRC16 1`
in `./include/protocol.h`

