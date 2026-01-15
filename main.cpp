#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "ANO Rotary Encoder Test" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << "========================" << std::endl;
    
    ANOEncoder encoder(0x49); // Default I2C address
    
    if (!encoder.begin()) {
        std::cerr << "Failed to initialize encoder!" << std::endl;
        return 1;
    }
    
    std::cout << "Encoder initialized successfully!" << std::endl;
    std::cout << "Firmware version: 0x" << std::hex << encoder.getVersion() << std::dec << std::endl;
    
    int32_t last_position = encoder.getEncoderPosition();
    std::cout << "Initial position: " << last_position << std::endl;
    std::cout << "\nWaiting for encoder rotation or button presses...\n" << std::endl;
    
    // Button state tracking for debouncing
    bool select_held = false;
    bool up_held = false;
    bool down_held = false;
    bool left_held = false;
    bool right_held = false;
    
    while (true) {
        // Check encoder position
        int32_t position = encoder.getEncoderPosition();
        if (position != last_position) {
            int32_t delta = position - last_position;
            std::cout << "ENCODER Position: " << position 
                      << " (delta: " << (delta > 0 ? "+" : "") << delta << ")" 
                      << std::endl;
            last_position = position;
        }
        
        // Check SELECT button with debouncing
        bool select_pressed = encoder.isSelectPressed();
        if (select_pressed && !select_held) {
            select_held = true;
            std::cout << "SELECT button PRESSED" << std::endl;
        } else if (!select_pressed && select_held) {
            select_held = false;
            std::cout << "SELECT button RELEASED" << std::endl;
        }
        
        // Check UP button with debouncing
        bool up_pressed = encoder.isUpPressed();
        if (up_pressed && !up_held) {
            up_held = true;
            std::cout << "UP button PRESSED" << std::endl;
        } else if (!up_pressed && up_held) {
            up_held = false;
            std::cout << "UP button RELEASED" << std::endl;
        }
        
        // Check DOWN button with debouncing
        bool down_pressed = encoder.isDownPressed();
        if (down_pressed && !down_held) {
            down_held = true;
            std::cout << "DOWN button PRESSED" << std::endl;
        } else if (!down_pressed && down_held) {
            down_held = false;
            std::cout << "DOWN button RELEASED" << std::endl;
        }
        
        // Check LEFT button with debouncing
        bool left_pressed = encoder.isLeftPressed();
        if (left_pressed && !left_held) {
            left_held = true;
            std::cout << "LEFT button PRESSED" << std::endl;
        } else if (!left_pressed && left_held) {
            left_held = false;
            std::cout << "LEFT button RELEASED" << std::endl;
        }
        
        // Check RIGHT button with debouncing
        bool right_pressed = encoder.isRightPressed();
        if (right_pressed && !right_held) {
            right_held = true;
            std::cout << "RIGHT button PRESSED" << std::endl;
        } else if (!right_pressed && right_held) {
            right_held = false;
            std::cout << "RIGHT button RELEASED" << std::endl;
        }
        
        usleep(10000); // 10ms delay
    }
    
    return 0;
}

