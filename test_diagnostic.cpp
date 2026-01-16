#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "ANO Encoder Diagnostic Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ANOEncoder encoder(0x49);
    
    if (!encoder.begin()) {
        std::cerr << "FAILED to initialize encoder!" << std::endl;
        std::cerr << "Check:" << std::endl;
        std::cerr << "  - I2C is enabled (sudo raspi-config)" << std::endl;
        std::cerr << "  - Wiring is correct" << std::endl;
        std::cerr << "  - Device is at address 0x49 (run: i2cdetect -y 1)" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Encoder initialized" << std::endl;
    std::cout << "✓ Firmware version: 0x" << std::hex << encoder.getVersion() << std::dec << std::endl;
    
    int32_t initial_position = encoder.getEncoderPosition();
    std::cout << "✓ Initial encoder position: " << initial_position << std::endl;
    std::cout << "\n========================================" << std::endl;
    std::cout << "Reading RAW button states every second..." << std::endl;
    std::cout << "This will help diagnose button issues" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int count = 0;
    while (true) {
        count++;
        
        // Read encoder
        int32_t position = encoder.getEncoderPosition();
        
        // Read all buttons
        bool select = encoder.isSelectPressed();
        bool up = encoder.isUpPressed();
        bool down = encoder.isDownPressed();
        bool left = encoder.isLeftPressed();
        bool right = encoder.isRightPressed();
        
        // Display status
        std::cout << "[" << count << "] "
                  << "Pos:" << position << " | "
                  << "SELECT:" << (select ? "YES" : "no ") << " | "
                  << "UP:" << (up ? "YES" : "no ") << " | "
                  << "DOWN:" << (down ? "YES" : "no ") << " | "
                  << "LEFT:" << (left ? "YES" : "no ") << " | "
                  << "RIGHT:" << (right ? "YES" : "no ")
                  << std::endl;
        
        sleep(1); // 1 second delay
    }
    
    return 0;
}


