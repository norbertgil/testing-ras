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
    
    // Uruchom Python script - użyj bezpośrednio pythona z venv
    std::cout << "Uruchamiam Python backend..." << std::endl;
    
    // Sprawdź czy venv istnieje
    if (access("venv/bin/python", X_OK) != 0) {
        std::cerr << "❌ Nie znaleziono venv/bin/python" << std::endl;
        std::cerr << "Uruchom najpierw: ./setup_python_env_system_packages.sh" << std::endl;
        return 1;
    }
    
    // Uruchom bezpośrednio z venv Python
    FILE* pipe = popen("venv/bin/python encoder_reader.py 2>&1", "r");
    if (!pipe) {
        std::cerr << "❌ Nie można uruchomić Python script!" << std::endl;
        return 1;
    }
    
    std::cout << "Python uruchomiony, czytam dane..." << std::endl;
    
    char buffer[256];
    bool ready = false;
    int last_position = 0;
    int line_count = 0;
    
    std::cout << "Czekam na inicjalizację...\n" << std::endl;
    
    while (running && fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        line_count++;
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
    int status = pclose(pipe);
    
    if (line_count == 0) {
        std::cerr << "\n❌ Python script nie zwrócił żadnych danych!" << std::endl;
        std::cerr << "Spróbuj uruchomić ręcznie:" << std::endl;
        std::cerr << "  venv/bin/python encoder_reader.py" << std::endl;
    }
    
    std::cout << "Zakończono (przeczytano " << line_count << " linii)." << std::endl;
    
    return 0;
}

