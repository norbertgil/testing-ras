#include "ano_encoder.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <iostream>

ANOEncoder::ANOEncoder(uint8_t address, const char* i2c_device)
    : device_address(address), i2c_fd(-1) {
    // Open I2C device
    i2c_fd = open(i2c_device, O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "Failed to open I2C device" << std::endl;
    }
}

ANOEncoder::~ANOEncoder() {
    if (i2c_fd >= 0) {
        close(i2c_fd);
    }
}

bool ANOEncoder::begin() {
    if (i2c_fd < 0) {
        return false;
    }
    
    // Set I2C slave address
    if (ioctl(i2c_fd, I2C_SLAVE, device_address) < 0) {
        std::cerr << "Failed to set I2C slave address" << std::endl;
        return false;
    }
    
    // Check version
    uint32_t version = getVersion();
    uint32_t product_id = (version >> 16) & 0xFFFF;
    
    if (product_id != 5740) {
        std::cerr << "Wrong firmware loaded? Expected 5740, got " << product_id << std::endl;
        return false;
    }
    
    // Configure button pins as inputs with pullups
    pinMode(SS_SWITCH_SELECT, 0x02); // INPUT_PULLUP
    pinMode(SS_SWITCH_UP, 0x02);
    pinMode(SS_SWITCH_LEFT, 0x02);
    pinMode(SS_SWITCH_DOWN, 0x02);
    pinMode(SS_SWITCH_RIGHT, 0x02);
    
    return true;
}

uint32_t ANOEncoder::getVersion() {
    uint8_t buffer[4];
    if (readRegister(SEESAW_STATUS_BASE, SEESAW_STATUS_VERSION, buffer, 4)) {
        return ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
               ((uint32_t)buffer[2] << 8) | buffer[3];
    }
    return 0;
}

int32_t ANOEncoder::getEncoderPosition() {
    uint8_t buffer[4];
    if (readRegister(SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION, buffer, 4)) {
        return ((int32_t)buffer[0] << 24) | ((int32_t)buffer[1] << 16) |
               ((int32_t)buffer[2] << 8) | buffer[3];
    }
    return 0;
}

int32_t ANOEncoder::getEncoderDelta() {
    uint8_t buffer[4];
    if (readRegister(SEESAW_ENCODER_BASE, SEESAW_ENCODER_DELTA, buffer, 4)) {
        return ((int32_t)buffer[0] << 24) | ((int32_t)buffer[1] << 16) |
               ((int32_t)buffer[2] << 8) | buffer[3];
    }
    return 0;
}

void ANOEncoder::pinMode(uint8_t pin, uint8_t mode) {
    uint8_t cmd[4];
    uint32_t pin_mask = 1 << pin;
    
    cmd[0] = (pin_mask >> 24) & 0xFF;
    cmd[1] = (pin_mask >> 16) & 0xFF;
    cmd[2] = (pin_mask >> 8) & 0xFF;
    cmd[3] = pin_mask & 0xFF;
    
    if (mode == 0x02) { // INPUT_PULLUP
        writeRegister(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
    }
}

bool ANOEncoder::digitalRead(uint8_t pin) {
    uint8_t buffer[4];
    if (readRegister(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buffer, 4)) {
        uint32_t value = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
                        ((uint32_t)buffer[2] << 8) | buffer[3];
        return (value & (1 << pin)) != 0;
    }
    return false;
}

bool ANOEncoder::isSelectPressed() { return !digitalRead(SS_SWITCH_SELECT); }
bool ANOEncoder::isUpPressed() { return !digitalRead(SS_SWITCH_UP); }
bool ANOEncoder::isDownPressed() { return !digitalRead(SS_SWITCH_DOWN); }
bool ANOEncoder::isLeftPressed() { return !digitalRead(SS_SWITCH_LEFT); }
bool ANOEncoder::isRightPressed() { return !digitalRead(SS_SWITCH_RIGHT); }

bool ANOEncoder::writeRegister(uint8_t reg_base, uint8_t reg_addr, uint8_t* data, uint8_t length) {
    uint8_t buffer[32];
    buffer[0] = reg_base;
    buffer[1] = reg_addr;
    memcpy(buffer + 2, data, length);
    
    return write(i2c_fd, buffer, length + 2) == (length + 2);
}

bool ANOEncoder::readRegister(uint8_t reg_base, uint8_t reg_addr, uint8_t* data, uint8_t length) {
    uint8_t command[2] = {reg_base, reg_addr};
    
    if (write(i2c_fd, command, 2) != 2) {
        return false;
    }
    
    usleep(1000); // Small delay for seesaw to process
    
    return read(i2c_fd, data, length) == length;
}

