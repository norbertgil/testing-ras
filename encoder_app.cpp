#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Prosty program C++ który uruchamia Python i czyta dane encodera

volatile bool running = true;

void signal_handler(int signal) {
    running = false;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "ANO Encoder - C++ + Python" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Obsługa Ctrl+C
    signal(SIGINT, signal_handler);
    
    // Uruchom Python script
    std::cout << "Uruchamiam Python backend..." << std::endl;
    
    FILE* pipe = popen("source venv/bin/activate 2>/dev/null && python encoder_reader.py 2>&1", "r");
    if (!pipe) {
        std::cerr << "❌ Nie można uruchomić Python script!" << std::endl;
        std::cerr << "Upewnij się że:\n";
        std::cerr << "  1. Środowisko venv jest skonfigurowane: ./setup_python_env_system_packages.sh\n";
        std::cerr << "  2. Plik encoder_reader.py istnieje\n";
        return 1;
    }
    
    char buffer[256];
    bool ready = false;
    int last_position = 0;
    
    std::cout << "Czekam na inicjalizację..." << std::endl;
    
    while (running && fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        // Usuń znak nowej linii
        buffer[strcspn(buffer, "\n")] = 0;
        
        std::string line(buffer);
        
        if (line == "READY") {
            ready = true;
            std::cout << "\n✓ Encoder gotowy!" << std::endl;
            std::cout << "Obracaj kółkiem lub naciskaj przyciski..." << std::endl;
            std::cout << "Ctrl+C aby zakończyć\n" << std::endl;
            continue;
        }
        
        if (!ready) {
            // Wyświetl komunikaty startowe
            std::cout << "  " << line << std::endl;
            continue;
        }
        
        // Parsuj dane
        if (line.find("ENCODER:") == 0) {
            int position = std::stoi(line.substr(8));
            std::cout << "🔄 Pozycja: " << position;
            if (last_position != 0) {
                int delta = position - last_position;
                std::cout << " (zmiana: " << (delta > 0 ? "+" : "") << delta << ")";
            }
            std::cout << std::endl;
            last_position = position;
        }
        else if (line.find("BUTTON:") == 0) {
            size_t colon1 = line.find(':', 7);
            if (colon1 != std::string::npos) {
                std::string button = line.substr(7, colon1 - 7);
                std::string action = line.substr(colon1 + 1);
                
                if (action == "PRESSED") {
                    std::cout << "🔘 Przycisk " << button << " naciśnięty" << std::endl;
                } else {
                    std::cout << "⚪ Przycisk " << button << " zwolniony" << std::endl;
                }
            }
        }
        else if (line.find("ERROR:") == 0) {
            std::cerr << "❌ Błąd: " << line.substr(6) << std::endl;
            break;
        }
    }
    
    std::cout << "\n\nZamykanie..." << std::endl;
    pclose(pipe);
    
    std::cout << "Zakończono." << std::endl;
    
    return 0;
}

