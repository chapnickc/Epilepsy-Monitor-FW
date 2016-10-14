# Epilepsy Monitor Firmware

## Repository Overview

This repository attempts to follow a basic directory tree organization, 
with libraries in `lib`,  source code in `src`, 
tests in `test`, etc. 
The `analysis` subdirectory currently consists of code for preliminary data processing. 
The `write` directory contains some TeX code for drawing circuit diagrams. 
The `tools` directory contains the [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded) needed 
to compile code for ARM processor used in this project. The toolcahin version is 4.9-2015-q3. 
I have attempted to use later versions, but have run into some issues. 


## Source Code
The code running on the ARM proccessor is C++. 
The heart rate calculations require the use of vectors from the C++11 standard library. 
I ran into some issues here when using the MBED compiler, as I believe it uses the C++98 standard library.
My solution to this was to export the program from the online MBED compiler to the ARM toolchain. This gave me a 
Makefile and all the relevant MBED libraries, such as the BLE_API, nrf51822, and mbed. All I had to do was add 
the path to the ARM toolchain, and change the `std` compiler flag to C++11. I also added an `upload` *function*. 
At the time of writing, the MBED libraries 
in this repository are out of date and should be exported again.

Once the code is ready to be compiled and uploaded to the device:
```
make
make merge
make upload
```
The first two commands place the output files in a directory entitled ".build" 
in the current working directory. The third command copys the 'combined.hex' to the device. 
This file that has been merged with the softdevice from the second command.

To clean the directory after the build:
```
make clean
```


## Connecting to the device
In order to read data from a serial port, connect the TX and RX pins on the BLE Nano to RX and TX on the MK20 (in that order).
To program the nRF51822 on the BLE Nano, connect the DCLK and DIO pins of the BLE Nano to the DCLK and DIO pins on the MK20.
DCLK DIO on the BLE Nano -> MK20 DCLK DIO

| BLE Nano Pin  |  MK20 Pin |
|:-------------:|:---------:|
|     TX        |   RX      |
|     RX        |   TX      |
|     DLCK      |   DCLK    |
|     DIO       |   DIO     |

