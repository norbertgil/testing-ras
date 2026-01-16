#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <cstdint>
#include <iomanip>

// Test raw I2C communication with the encoder
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "RAW I2C Communication Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Open I2C device
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "❌ Failed to open /dev/i2c-1" << std::endl;
        std::cerr << "Make sure I2C is enabled!" << std::endl;
        return 1;
    }
    std::cout << "✓ Opened /dev/i2c-1" << std::endl;
    
    // Set I2C slave address
    if (ioctl(i2c_fd, I2C_SLAVE, 0x49) < 0) {
        std::cerr << "❌ Failed to set I2C address to 0x49" << std::endl;
        close(i2c_fd);
        return 1;
    }
    std::cout << "✓ Set I2C address to 0x49" << std::endl;
    
    // Read version (register 0x00, 0x02)
    uint8_t cmd[2] = {0x00, 0x02};
    if (write(i2c_fd, cmd, 2) != 2) {
        std::cerr << "❌ Failed to write version command" << std::endl;
        close(i2c_fd);
        return 1;
    }
    std::cout << "✓ Sent version command" << std::endl;
    
    usleep(10000);
    
    uint8_t version_buf[4];
    if (read(i2c_fd, version_buf, 4) != 4) {
        std::cerr << "❌ Failed to read version" << std::endl;
        close(i2c_fd);
        return 1;
    }
    
    uint32_t version = ((uint32_t)version_buf[0] << 24) | ((uint32_t)version_buf[1] << 16) |
                       ((uint32_t)version_buf[2] << 8) | version_buf[3];
    uint32_t product_id = (version >> 16) & 0xFFFF;
    
    std::cout << "✓ Version: 0x" << std::hex << version << std::dec << std::endl;
    std::cout << "✓ Product ID: " << product_id << std::endl;
    
    if (product_id == 5740) {
        std::cout << "✓ Correct product ID!" << std::endl;
    } else {
        std::cout << "⚠️  WARNING: Expected 5740, got " << product_id << std::endl;
    }
    
    // Enable encoder (register 0x11, 0x10, value 0x01)
    std::cout << "\nEnabling encoder..." << std::endl;
    uint8_t enable_cmd[3] = {0x11, 0x10, 0x01};
    if (write(i2c_fd, enable_cmd, 3) != 3) {
        std::cerr << "❌ Failed to enable encoder" << std::endl;
    } else {
        std::cout << "✓ Encoder enabled" << std::endl;
    }
    
    usleep(50000);
    
    // Set encoder position to 0
    std::cout << "Setting encoder position to 0..." << std::endl;
    uint8_t set_pos_cmd[6] = {0x11, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (write(i2c_fd, set_pos_cmd, 6) != 6) {
        std::cerr << "❌ Failed to set encoder position" << std::endl;
    } else {
        std::cout << "✓ Encoder position set to 0" << std::endl;
    }
    
    usleep(50000);
    
    // Read encoder position continuously
    std::cout << "\n========================================" << std::endl;
    std::cout << "Reading encoder position (rotate wheel!)" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int32_t last_position = 0;
    int reading_count = 0;
    
    while (true) {
        // Read encoder position (register 0x11, 0x00)
        uint8_t read_cmd[2] = {0x11, 0x00};
        if (write(i2c_fd, read_cmd, 2) == 2) {
            usleep(2000);
            
            uint8_t pos_buf[4];
            if (read(i2c_fd, pos_buf, 4) == 4) {
                int32_t position = ((int32_t)pos_buf[0] << 24) | ((int32_t)pos_buf[1] << 16) |
                                  ((int32_t)pos_buf[2] << 8) | pos_buf[3];
                
                if (position != last_position) {
                    reading_count++;
                    std::cout << "[" << std::setw(4) << reading_count << "] "
                              << "Position: " << std::setw(6) << position
                              << " (Raw bytes: "
                              << std::hex << std::setfill('0')
                              << std::setw(2) << (int)pos_buf[0] << " "
                              << std::setw(2) << (int)pos_buf[1] << " "
                              << std::setw(2) << (int)pos_buf[2] << " "
                              << std::setw(2) << (int)pos_buf[3]
                              << std::dec << std::setfill(' ') << ")"
                              << std::endl;
                    last_position = position;
                }
            }
        }
        
        usleep(10000); // 10ms delay
    }
    
    close(i2c_fd);
    return 0;
}

