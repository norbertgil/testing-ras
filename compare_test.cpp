#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "C++ vs Python Comparison Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nSince Python works, let's debug C++...\n" << std::endl;
    
    ANOEncoder encoder(0x49);
    
    if (!encoder.begin()) {
        std::cerr << "❌ Failed to initialize encoder!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Encoder initialized" << std::endl;
    std::cout << "✓ Firmware version: 0x" << std::hex << encoder.getVersion() << std::dec << std::endl;
    
    // Try reading position multiple times with delays
    std::cout << "\nReading encoder position 10 times..." << std::endl;
    for (int i = 0; i < 10; i++) {
        int32_t pos = encoder.getEncoderPosition();
        std::cout << "  Read " << i+1 << ": Position = " << pos << std::endl;
        usleep(100000); // 100ms delay
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Now ROTATE the wheel and watch..." << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int32_t last_position = encoder.getEncoderPosition();
    int read_count = 0;
    
    while (true) {
        int32_t position = encoder.getEncoderPosition();
        read_count++;
        
        // Print every read to see if anything changes
        if (read_count % 100 == 0) {
            std::cout << "Still reading... (count: " << read_count 
                      << ", position: " << position << ")" << std::endl;
        }
        
        if (position != last_position) {
            std::cout << "\n🎉 POSITION CHANGED!" << std::endl;
            std::cout << "   From: " << last_position << " → To: " << position << std::endl;
            std::cout << "   Delta: " << (position - last_position) << "\n" << std::endl;
            last_position = position;
        }
        
        usleep(5000); // 5ms
    }
    
    return 0;
}

