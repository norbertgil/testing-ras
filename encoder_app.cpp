#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <map>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

volatile bool running = true;

void signal_handler(int signal) {
    running = false;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Encodery - ANO + 2x ENC + BTN (MCP23017)" << std::endl;
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
    int line_count = 0;

    std::map<std::string, int> enc_positions;
    const char* enc_names[] = {"ENCODER", "ENC2", "ENC3"};
    for (auto name : enc_names) enc_positions[name] = 0;
    
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
        
        // Parsuj encodery (ENC2:, ENC3:, ENC4:, ENC5:, ENCODER:)
        bool handled = false;
        for (auto& [name, last_pos] : enc_positions) {
            std::string prefix = name + ":";
            if (line.find(prefix) == 0) {
                int position = std::stoi(line.substr(prefix.size()));
                int delta = position - last_pos;
                std::cout << "🔄 " << name << ": " << position;
                if (last_pos != 0)
                    std::cout << " (" << (delta > 0 ? "+" : "") << delta << ")";
                std::cout << std::endl;
                last_pos = position;
                handled = true;
                break;
            }
        }
        if (handled) {}
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

