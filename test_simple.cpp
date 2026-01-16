#include "ano_encoder.h"
#include <iostream>
#include <unistd.h>

// Prosty test - dokładnie jak Python
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Prosty test enkodera - jak Python" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    ANOEncoder encoder(0x49);
    
    if (!encoder.begin()) {
        std::cerr << "\n❌ Nie udało się zainicjalizować encodera!" << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ Encoder zainicjalizowany!" << std::endl;
    std::cout << "\n========================================" << std::endl;
    std::cout << "OBRACAJ KÓŁKO!" << std::endl;
    std::cout << "Ctrl+C aby wyjść" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int32_t last_position = -999999; // Specjalna wartość żeby wyświetlić pierwszą pozycję
    int loop_count = 0;
    
    while (true) {
        // Dokładnie jak Python: position = encoder.position
        int32_t position = encoder.getEncoderPosition();
        
        loop_count++;
        if (loop_count % 200 == 0) {
            std::cout << "  [Czytam... pozycja: " << position << "]" << std::endl;
        }
        
        // Dokładnie jak Python: if position != last_position:
        if (position != last_position) {
            std::cout << "Position: " << position << std::endl;
            last_position = position;
        }
        
        // Krótkie opóźnienie (Python też ma pętlę bez sleep, ale my dodamy małe)
        usleep(5000); // 5ms
    }
    
    return 0;
}

