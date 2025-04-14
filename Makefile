# Project name
PROJECT = main

# Target MCU and clock speed
MCU = atmega328p
F_CPU = 16000000UL

# AVR tools
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Programmer settings
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 57600

# Default target
all: $(BUILD_DIR)/$(PROJECT).hex

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link the object files
$(BUILD_DIR)/$(PROJECT).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Convert to HEX
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Upload to Arduino
upload: $(BUILD_DIR)/$(PROJECT).hex
	$(AVRDUDE) -V -F -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all upload clean
