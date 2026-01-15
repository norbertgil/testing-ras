#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "ANO Rotary Encoder Test" << std::endl;
    
    ANOEncoder encoder(0x49); // Default I2C address
    
    if (!encoder.begin()) {
        std::cerr << "Failed to initialize encoder!" << std::endl;
        return 1;
    }
    
    std::cout << "Encoder initialized successfully!" << std::endl;
    std::cout << "Firmware version: 0x" << std::hex << encoder.getVersion() << std::dec << std::endl;
    
    int32_t last_position = encoder.getEncoderPosition();
    std::cout << "Initial position: " << last_position << std::endl;
    
    while (true) {
        // Check encoder position
        int32_t position = encoder.getEncoderPosition();
        if (position != last_position) {
            std::cout << "Position: " << position << std::endl;
            last_position = position;
        }
        
        // Check buttons
        if (encoder.isSelectPressed()) {
            std::cout << "SELECT pressed!" << std::endl;
        }
        if (encoder.isUpPressed()) {
            std::cout << "UP pressed!" << std::endl;
        }
        if (encoder.isDownPressed()) {
            std::cout << "DOWN pressed!" << std::endl;
        }
        if (encoder.isLeftPressed()) {
            std::cout << "LEFT pressed!" << std::endl;
        }
        if (encoder.isRightPressed()) {
            std::cout << "RIGHT pressed!" << std::endl;
        }
        
        usleep(10000); // 10ms delay
    }
    
    return 0;
}

