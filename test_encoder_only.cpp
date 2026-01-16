#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "ANO Encoder - Scroll Wheel Only Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    ANOEncoder encoder(0x49);
    
    if (!encoder.begin()) {
        std::cerr << "\n❌ FAILED to initialize encoder!" << std::endl;
        std::cerr << "\nTroubleshooting steps:" << std::endl;
        std::cerr << "1. Check I2C is enabled: sudo raspi-config" << std::endl;
        std::cerr << "2. Check wiring connections" << std::endl;
        std::cerr << "3. Run: i2cdetect -y 1" << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ Encoder initialized successfully!" << std::endl;
    
    uint32_t version = encoder.getVersion();
    uint32_t product_id = (version >> 16) & 0xFFFF;
    std::cout << "✓ Firmware version: 0x" << std::hex << version << std::dec << std::endl;
    std::cout << "✓ Product ID: " << product_id << " (expected: 5740)" << std::endl;
    
    // Wait a moment for encoder to stabilize
    usleep(100000);
    
    int32_t position = encoder.getEncoderPosition();
    std::cout << "✓ Current encoder position: " << position << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "ROTATE THE SCROLL WHEEL NOW!" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Press Ctrl+C to exit\n" << std::endl;
    
    int32_t last_position = position;
    int update_count = 0;
    
    while (true) {
        position = encoder.getEncoderPosition();
        
        if (position != last_position) {
            int32_t delta = position - last_position;
            update_count++;
            
            std::cout << std::setw(4) << update_count << ": "
                      << "Position = " << std::setw(6) << position
                      << " | Change = " << std::setw(+3) << (delta > 0 ? "+" : "") << delta
                      << " | Direction: " << (delta > 0 ? "CLOCKWISE ↻" : "COUNTER-CLOCKWISE ↺")
                      << std::endl;
            
            last_position = position;
        }
        
        usleep(5000); // 5ms delay - faster polling
    }
    
    return 0;
}

