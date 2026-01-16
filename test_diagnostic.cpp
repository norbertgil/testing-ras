#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// Direct I2C read for debugging
bool readI2CRegister(int fd, uint8_t reg_base, uint8_t reg_addr, uint8_t* buffer, size_t len) {
    uint8_t cmd[2] = {reg_base, reg_addr};
    if (write(fd, cmd, 2) != 2) {
        return false;
    }
    usleep(2000); // 2ms delay
    return read(fd, buffer, len) == (ssize_t)len;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "ANO Encoder DETAILED Diagnostic Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Try to open I2C device directly
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "✗ Cannot open /dev/i2c-1" << std::endl;
        std::cerr << "  Make sure I2C is enabled!" << std::endl;
        return 1;
    }
    std::cout << "✓ I2C device opened" << std::endl;
    
    // Set address
    if (ioctl(i2c_fd, I2C_SLAVE, 0x49) < 0) {
        std::cerr << "✗ Cannot set I2C address to 0x49" << std::endl;
        std::cerr << "  Run: i2cdetect -y 1" << std::endl;
        close(i2c_fd);
        return 1;
    }
    std::cout << "✓ I2C address set to 0x49" << std::endl;
    
    // Read version directly
    uint8_t version_buf[4];
    if (readI2CRegister(i2c_fd, 0x00, 0x02, version_buf, 4)) {
        uint32_t version = ((uint32_t)version_buf[0] << 24) | ((uint32_t)version_buf[1] << 16) |
                          ((uint32_t)version_buf[2] << 8) | version_buf[3];
        uint16_t product_id = (version >> 16) & 0xFFFF;
        std::cout << "✓ Raw firmware version: 0x" << std::hex << version << std::dec << std::endl;
        std::cout << "✓ Product ID: " << product_id << (product_id == 5740 ? " (CORRECT!)" : " (WRONG!)") << std::endl;
    } else {
        std::cerr << "✗ Cannot read version register" << std::endl;
    }
    
    close(i2c_fd);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Initializing with ANOEncoder class..." << std::endl;
    std::cout << "========================================" << std::endl;
    
    ANOEncoder encoder(0x49);
    
    if (!encoder.begin()) {
        std::cerr << "✗ FAILED to initialize encoder!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Encoder initialized" << std::endl;
    std::cout << "✓ Firmware version: 0x" << std::hex << encoder.getVersion() << std::dec << std::endl;
    
    // Try to read encoder multiple times
    std::cout << "\n========================================" << std::endl;
    std::cout << "Testing encoder position reads..." << std::endl;
    std::cout << "========================================" << std::endl;
    
    for (int i = 0; i < 5; i++) {
        int32_t pos = encoder.getEncoderPosition();
        int32_t delta = encoder.getEncoderDelta();
        std::cout << "Read " << (i+1) << ": Position = " << pos << ", Delta = " << delta << std::endl;
        usleep(200000); // 200ms
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "ROTATE THE ENCODER NOW!" << std::endl;
    std::cout << "Monitoring every 100ms for 10 seconds..." << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int32_t last_position = encoder.getEncoderPosition();
    int changes = 0;
    
    for (int i = 0; i < 100; i++) {  // 10 seconds total
        int32_t position = encoder.getEncoderPosition();
        int32_t delta = encoder.getEncoderDelta();
        
        if (position != last_position) {
            changes++;
            std::cout << ">>> ENCODER MOVED! Position: " << position 
                      << " (was: " << last_position << ", delta: " << delta << ")" << std::endl;
            last_position = position;
        } else if (i % 10 == 0) {
            std::cout << "[" << (i/10) << "s] Position: " << position << " (no change)" << std::endl;
        }
        
        usleep(100000); // 100ms
    }
    
    std::cout << "\n========================================" << std::endl;
    if (changes > 0) {
        std::cout << "✓ SUCCESS! Encoder responded " << changes << " times!" << std::endl;
    } else {
        std::cout << "✗ PROBLEM: Encoder did not respond" << std::endl;
        std::cout << "  Check:" << std::endl;
        std::cout << "  - Is the encoder properly soldered?" << std::endl;
        std::cout << "  - Are the encoder pins making good contact?" << std::endl;
        std::cout << "  - Try rotating it multiple full turns" << std::endl;
    }
    std::cout << "========================================" << std::endl;
    
    std::cout << "\nNow testing buttons (press any button)..." << std::endl;
    std::cout << "Testing for 10 seconds..." << std::endl;
    
    for (int i = 0; i < 100; i++) {
        bool select = encoder.isSelectPressed();
        bool up = encoder.isUpPressed();
        bool down = encoder.isDownPressed();
        bool left = encoder.isLeftPressed();
        bool right = encoder.isRightPressed();
        
        if (select || up || down || left || right) {
            std::cout << "BUTTON: ";
            if (select) std::cout << "SELECT ";
            if (up) std::cout << "UP ";
            if (down) std::cout << "DOWN ";
            if (left) std::cout << "LEFT ";
            if (right) std::cout << "RIGHT ";
            std::cout << std::endl;
        }
        
        usleep(100000); // 100ms
    }
    
    std::cout << "\nDiagnostic complete!" << std::endl;
    
    return 0;
}


