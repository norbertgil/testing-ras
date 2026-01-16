#include "ano_encoder.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <iostream>

ANOEncoder::ANOEncoder(uint8_t address, const char* i2c_device)
    : i2c_fd(-1), device_address(address) {
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
    
    std::cout << "DEBUG: I2C address set to 0x" << std::hex << (int)device_address << std::dec << std::endl;
    
    // Small delay after opening
    usleep(10000);
    
    // Check version (dokładnie jak Python: seesaw.get_version())
    uint32_t version = getVersion();
    uint32_t product_id = (version >> 16) & 0xFFFF;
    
    std::cout << "DEBUG: Version = 0x" << std::hex << version << std::dec << std::endl;
    std::cout << "DEBUG: Product ID = " << product_id << std::endl;
    
    if (product_id != 5740) {
        std::cerr << "Wrong firmware loaded? Expected 5740, got " << product_id << std::endl;
        return false;
    }
    
    // Configure button pins as inputs with pullups (dokładnie jak Python: seesaw.pin_mode(..., seesaw.INPUT_PULLUP))
    std::cout << "DEBUG: Configuring button pins..." << std::endl;
    pinMode(SS_SWITCH_SELECT, 0x02); // INPUT_PULLUP
    pinMode(SS_SWITCH_UP, 0x02);
    pinMode(SS_SWITCH_LEFT, 0x02);
    pinMode(SS_SWITCH_DOWN, 0x02);
    pinMode(SS_SWITCH_RIGHT, 0x02);
    
    usleep(10000);
    
    // Python robi: encoder = rotaryio.IncrementalEncoder(seesaw)
    // To wewnętrznie NIE ROBI nic specjalnego - encoder jest już gotowy!
    // Nie trzeba go "enableować" - po prostu czytamy pozycję
    
    std::cout << "DEBUG: Encoder initialized" << std::endl;
    
    // Spróbuj przeczytać początkową pozycję
    int32_t initial_pos = getEncoderPosition();
    std::cout << "DEBUG: Initial encoder position = " << initial_pos << std::endl;
    
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
    // Dokładnie jak Python: encoder.position
    // Czyta z rejestru SEESAW_ENCODER_BASE (0x11), SEESAW_ENCODER_POSITION (0x00)
    if (readRegister(SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION, buffer, 4)) {
        int32_t position = ((int32_t)buffer[0] << 24) | ((int32_t)buffer[1] << 16) |
                          ((int32_t)buffer[2] << 8) | buffer[3];
        return position;
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

void ANOEncoder::setEncoderPosition(int32_t position) {
    uint8_t buffer[4];
    buffer[0] = (position >> 24) & 0xFF;
    buffer[1] = (position >> 16) & 0xFF;
    buffer[2] = (position >> 8) & 0xFF;
    buffer[3] = position & 0xFF;
    writeRegister(SEESAW_ENCODER_BASE, SEESAW_ENCODER_POSITION, buffer, 4);
}

void ANOEncoder::enableEncoder() {
    // Python NIE wywołuje żadnej enable funkcji!
    // Encoder jest gotowy od razu po inicjalizacji seesaw
    // Ta funkcja może być pusta lub usunięta
    std::cout << "DEBUG: enableEncoder() called (may not be necessary)" << std::endl;
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
    
    ssize_t written = write(i2c_fd, buffer, length + 2);
    if (written != (length + 2)) {
        std::cerr << "DEBUG: Write failed, expected " << (length + 2) << " bytes, wrote " << written << std::endl;
        return false;
    }
    return true;
}

bool ANOEncoder::readRegister(uint8_t reg_base, uint8_t reg_addr, uint8_t* data, uint8_t length) {
    uint8_t command[2] = {reg_base, reg_addr};
    
    // Write command
    if (write(i2c_fd, command, 2) != 2) {
        std::cerr << "DEBUG: Failed to write read command" << std::endl;
        return false;
    }
    
    // Delay for seesaw to process (Python też ma to w swojej bibliotece)
    usleep(2000); // 2ms - zwiększony z 1ms
    
    // Read response
    ssize_t bytes_read = read(i2c_fd, data, length);
    if (bytes_read != length) {
        std::cerr << "DEBUG: Read failed, expected " << (int)length << " bytes, got " << bytes_read << std::endl;
        return false;
    }
    
    return true;
}

