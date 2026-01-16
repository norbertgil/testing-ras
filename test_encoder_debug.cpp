#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <cstdint>
#include <cstring>
#include <errno.h>

// Zaawansowany test encodera z różnymi metodami I2C
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Debugowanie problemu z encoderem" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "❌ Nie można otworzyć I2C" << std::endl;
        return 1;
    }
    
    if (ioctl(i2c_fd, I2C_SLAVE, 0x49) < 0) {
        std::cerr << "❌ Nie można ustawić adresu" << std::endl;
        close(i2c_fd);
        return 1;
    }
    
    std::cout << "✓ I2C otwarty na 0x49\n" << std::endl;
    usleep(100000);
    
    // 1. SOFT RESET
    std::cout << "1. SOFT RESET..." << std::endl;
    uint8_t reset_cmd[3] = {0x00, 0x7F, 0xFF};
    if (write(i2c_fd, reset_cmd, 3) == 3) {
        std::cout << "   ✓ Reset wysłany" << std::endl;
        sleep(1);
        ioctl(i2c_fd, I2C_SLAVE, 0x49);
        usleep(100000);
    }
    
    // 2. Sprawdź wersję
    std::cout << "\n2. Sprawdzenie wersji..." << std::endl;
    uint8_t ver_cmd[2] = {0x00, 0x02};
    write(i2c_fd, ver_cmd, 2);
    usleep(10000);
    uint8_t ver_buf[4];
    if (read(i2c_fd, ver_buf, 4) == 4) {
        uint32_t version = ((uint32_t)ver_buf[0] << 24) | ((uint32_t)ver_buf[1] << 16) |
                          ((uint32_t)ver_buf[2] << 8) | ver_buf[3];
        std::cout << "   ✓ Version: 0x" << std::hex << version << std::dec << std::endl;
    }
    
    // 3. Włącz encoder - użyj I2C_RDWR dla pewności
    std::cout << "\n3. Włączanie encodera (metoda I2C_RDWR)..." << std::endl;
    
    struct i2c_msg enable_msgs[1];
    uint8_t enable_data[6] = {0x11, 0x10, 0x00, 0x00, 0x00, 0x01};
    
    enable_msgs[0].addr = 0x49;
    enable_msgs[0].flags = 0; // Write
    enable_msgs[0].len = 6;
    enable_msgs[0].buf = enable_data;
    
    struct i2c_rdwr_ioctl_data enable_ioctl = {
        .msgs = enable_msgs,
        .nmsgs = 1
    };
    
    if (ioctl(i2c_fd, I2C_RDWR, &enable_ioctl) < 0) {
        std::cerr << "   ✗ Nie można włączyć encodera: " << strerror(errno) << std::endl;
    } else {
        std::cout << "   ✓ Encoder włączony (I2C_RDWR)" << std::endl;
        usleep(100000);
    }
    
    // 4. Ustaw pozycję początkową
    std::cout << "\n4. Ustawianie pozycji początkowej..." << std::endl;
    uint8_t setpos_data[6] = {0x11, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    struct i2c_msg setpos_msgs[1];
    setpos_msgs[0].addr = 0x49;
    setpos_msgs[0].flags = 0;
    setpos_msgs[0].len = 6;
    setpos_msgs[0].buf = setpos_data;
    
    struct i2c_rdwr_ioctl_data setpos_ioctl = {
        .msgs = setpos_msgs,
        .nmsgs = 1
    };
    
    if (ioctl(i2c_fd, I2C_RDWR, &setpos_ioctl) < 0) {
        std::cerr << "   ✗ Nie można ustawić pozycji: " << strerror(errno) << std::endl;
    } else {
        std::cout << "   ✓ Pozycja ustawiona" << std::endl;
        usleep(100000);
    }
    
    // 5. Odczyt pozycji - metoda I2C_RDWR (write + read atomowo)
    std::cout << "\n5. Odczyt pozycji encodera (I2C_RDWR)..." << std::endl;
    
    struct i2c_msg msgs[2];
    uint8_t read_cmd[2] = {0x11, 0x00};
    uint8_t pos_buf[4] = {0};
    
    // Wiadomość 1: Write command
    msgs[0].addr = 0x49;
    msgs[0].flags = 0; // Write
    msgs[0].len = 2;
    msgs[0].buf = read_cmd;
    
    // Wiadomość 2: Read response
    msgs[1].addr = 0x49;
    msgs[1].flags = I2C_M_RD; // Read
    msgs[1].len = 4;
    msgs[1].buf = pos_buf;
    
    struct i2c_rdwr_ioctl_data rdwr_data = {
        .msgs = msgs,
        .nmsgs = 2
    };
    
    int result = ioctl(i2c_fd, I2C_RDWR, &rdwr_data);
    if (result < 0) {
        std::cerr << "   ❌ I2C_RDWR failed: " << strerror(errno) << std::endl;
        
        // Spróbuj starej metody z opóźnieniem
        std::cout << "\n6. Próba ze standardowym write/read..." << std::endl;
        write(i2c_fd, read_cmd, 2);
        usleep(20000); // 20ms opóźnienie
        
        ssize_t bytes = read(i2c_fd, pos_buf, 4);
        if (bytes == 4) {
            int32_t position = ((int32_t)pos_buf[0] << 24) | ((int32_t)pos_buf[1] << 16) |
                              ((int32_t)pos_buf[2] << 8) | pos_buf[3];
            std::cout << "   ✓ Pozycja (stara metoda): " << position << std::endl;
        } else {
            std::cerr << "   ❌ Też nie działa, bytes=" << bytes << ": " << strerror(errno) << std::endl;
        }
    } else {
        int32_t position = ((int32_t)pos_buf[0] << 24) | ((int32_t)pos_buf[1] << 16) |
                          ((int32_t)pos_buf[2] << 8) | pos_buf[3];
        std::cout << "   🎉 Pozycja: " << position << std::endl;
        
        // Jeśli działa, spróbuj pętli
        std::cout << "\n7. Test ciągłego odczytu (obracaj kółkiem!)..." << std::endl;
        std::cout << "   Ctrl+C aby przerwać\n" << std::endl;
        
        int32_t last_pos = position;
        int count = 0;
        
        while (true) {
            result = ioctl(i2c_fd, I2C_RDWR, &rdwr_data);
            if (result >= 0) {
                position = ((int32_t)pos_buf[0] << 24) | ((int32_t)pos_buf[1] << 16) |
                          ((int32_t)pos_buf[2] << 8) | pos_buf[3];
                
                if (position != last_pos) {
                    std::cout << "   Position: " << position << std::endl;
                    last_pos = position;
                }
                
                count++;
                if (count % 100 == 0) {
                    std::cout << "   [Czytam... " << count << " odczytów, pozycja: " << position << "]" << std::endl;
                }
            }
            
            usleep(5000); // 5ms
        }
    }
    
    close(i2c_fd);
    return 0;
}

