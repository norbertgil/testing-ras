#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <cstring>
#include <errno.h>

// Test z soft reset - może to jest klucz!
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Test z Soft Reset seesaw" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Otwórz I2C
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "❌ Nie można otworzyć /dev/i2c-1: " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "✓ Otworzyłem /dev/i2c-1" << std::endl;
    
    // Ustaw adres
    if (ioctl(i2c_fd, I2C_SLAVE, 0x49) < 0) {
        std::cerr << "❌ Nie można ustawić adresu 0x49: " << strerror(errno) << std::endl;
        close(i2c_fd);
        return 1;
    }
    std::cout << "✓ Ustawiono adres 0x49" << std::endl;
    
    // Czekaj chwilę
    std::cout << "\nCzekam 100ms..." << std::endl;
    usleep(100000);
    
    // Spróbuj SOFT RESET (Python może to robić!)
    // Rejestr: 0x00 (STATUS_BASE), 0x7F (SWRST), wartość: 0xFF
    std::cout << "\n>>> Wysyłam SOFT RESET do seesaw..." << std::endl;
    uint8_t reset_cmd[3] = {0x00, 0x7F, 0xFF};
    ssize_t written = write(i2c_fd, reset_cmd, 3);
    if (written != 3) {
        std::cerr << "⚠️  Reset write failed, wrote " << written << " bytes: " << strerror(errno) << std::endl;
        std::cout << "   Kontynuuję mimo to..." << std::endl;
    } else {
        std::cout << "✓ Reset wysłany, czekam 1 sekundę na restart seesaw..." << std::endl;
        sleep(1);
        
        // Po resecie może trzeba ponownie ustawić adres
        if (ioctl(i2c_fd, I2C_SLAVE, 0x49) < 0) {
            std::cerr << "❌ Nie można ponownie ustawić adresu po reset" << std::endl;
            close(i2c_fd);
            return 1;
        }
        std::cout << "✓ Ponownie ustawiono adres po reset" << std::endl;
        usleep(100000);
    }
    
    // Teraz spróbuj odczytać wersję
    std::cout << "\n>>> Próba odczytu wersji (0x00:0x02)..." << std::endl;
    uint8_t version_cmd[2] = {0x00, 0x02};
    written = write(i2c_fd, version_cmd, 2);
    if (written != 2) {
        std::cerr << "❌ Write version command failed: " << strerror(errno) << std::endl;
    } else {
        std::cout << "✓ Komenda wysłana, czekam na odpowiedź..." << std::endl;
        usleep(10000); // 10ms
        
        uint8_t version_buf[4];
        ssize_t bytes_read = read(i2c_fd, version_buf, 4);
        if (bytes_read != 4) {
            std::cerr << "❌ Read version failed, got " << bytes_read << " bytes: " << strerror(errno) << std::endl;
        } else {
            uint32_t version = ((uint32_t)version_buf[0] << 24) | 
                              ((uint32_t)version_buf[1] << 16) |
                              ((uint32_t)version_buf[2] << 8) | 
                              version_buf[3];
            uint32_t product_id = (version >> 16) & 0xFFFF;
            
            std::cout << "✓ Odczytano wersję!" << std::endl;
            std::cout << "  Version: 0x" << std::hex << version << std::dec << std::endl;
            std::cout << "  Product ID: " << product_id << std::endl;
            
            if (product_id == 5740) {
                std::cout << "\n🎉 SUKCES! To jest ANO Encoder!" << std::endl;
            } else {
                std::cout << "\n⚠️  Dziwny product ID, oczekiwano 5740" << std::endl;
            }
        }
    }
    
    // Teraz spróbuj odczytać pozycję encodera
    std::cout << "\n>>> Próba odczytu pozycji encodera (0x11:0x00)..." << std::endl;
    uint8_t encoder_cmd[2] = {0x11, 0x00};
    written = write(i2c_fd, encoder_cmd, 2);
    if (written != 2) {
        std::cerr << "❌ Write encoder command failed: " << strerror(errno) << std::endl;
    } else {
        std::cout << "✓ Komenda wysłana, czekam na odpowiedź..." << std::endl;
        usleep(10000);
        
        uint8_t pos_buf[4];
        ssize_t bytes_read = read(i2c_fd, pos_buf, 4);
        if (bytes_read != 4) {
            std::cerr << "❌ Read encoder failed, got " << bytes_read << " bytes: " << strerror(errno) << std::endl;
        } else {
            int32_t position = ((int32_t)pos_buf[0] << 24) | 
                              ((int32_t)pos_buf[1] << 16) |
                              ((int32_t)pos_buf[2] << 8) | 
                              pos_buf[3];
            
            std::cout << "✓ Odczytano pozycję: " << position << std::endl;
            std::cout << "\n🎉 ENCODER DZIAŁA W C++!" << std::endl;
        }
    }
    
    close(i2c_fd);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Koniec testu" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

