PROJECT_DIR    = /Users/chapnickc/repos/LifeBud_v2.0
BOARD_TAG      = uno

MONITOR_BAUDRATE  = 9600
MONITOR_PORT      = /dev/tty.usbmodem*     # Using an '*' tries all the ports


# for using arduino makefile 
ARDUINO_DIR    = /Applications/Arduino.app/Contents/Java
ARDMK_DIR      = /usr/local/opt/arduino-mk

AVR_TOOLS_DIR  = /usr/local
AVR_TOOLS_PATH = /usr/local/bin
AVRDUDE        = /usr/local/bin/avrdude

# Check if version is >= 4.9
AVR_GCC_VERSION  := $(shell expr `avr-gcc -dumpversion | cut -f1` \>= 4.9)

CFLAGS_STD        = -std=gnu11 
CXXFLAGS_STD      = -std=gnu++11
CXXFLAGS          = -pedantic -Wall -Wextra # Flags for debugging purpose

# If avr-gcc -v is higher than 4.9, activate coloring of the output
ifeq "$(AVR_GCC_VERSION)" "1"
	CXXFLAGS += -fdiagnostics-color
endif

# This is were you put the binaries you just compile using 'make'
OBJDIR            = $(PROJECT_DIR)/bin/$(CURRENT_DIR)/$(BOARD_TAG)

USER_LIB_PATH  :=  $(realpath $(PROJECT_DIR)/lib)
CURRENT_DIR    = $(shell basename $(CURDIR))


include $(ARDMK_DIR)/Arduino.mk # path to Arduino.mk, inside the ARDMK_DIR
