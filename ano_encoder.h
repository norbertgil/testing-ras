#ifndef ANO_ENCODER_H
#define ANO_ENCODER_H

#include <cstdint>
#include <string>

// Seesaw register addresses
#define SEESAW_STATUS_BASE 0x00
#define SEESAW_GPIO_BASE 0x01
#define SEESAW_ENCODER_BASE 0x11

#define SEESAW_STATUS_VERSION 0x02
#define SEESAW_STATUS_SWRST 0x7F
#define SEESAW_GPIO_DIRSET_BULK 0x02
#define SEESAW_GPIO_BULK 0x04
#define SEESAW_GPIO_PULLENSET 0x0B
#define SEESAW_ENCODER_POSITION 0x00
#define SEESAW_ENCODER_DELTA 0x01
#define SEESAW_ENCODER_INTENSET 0x10
#define SEESAW_ENCODER_INTENCLR 0x20

// Button pins
#define SS_SWITCH_SELECT 1
#define SS_SWITCH_UP 2
#define SS_SWITCH_LEFT 3
#define SS_SWITCH_DOWN 4
#define SS_SWITCH_RIGHT 5

class ANOEncoder {
public:
    ANOEncoder(uint8_t address = 0x49, const char* i2c_device = "/dev/i2c-1");
    ~ANOEncoder();
    
    bool begin();
    uint32_t getVersion();
    
    // Encoder functions
    int32_t getEncoderPosition();
    int32_t getEncoderDelta();
    void setEncoderPosition(int32_t position);
    void enableEncoder();
    
    // GPIO functions
    void pinMode(uint8_t pin, uint8_t mode);
    bool digitalRead(uint8_t pin);
    
    // Button functions
    bool isSelectPressed();
    bool isUpPressed();
    bool isDownPressed();
    bool isLeftPressed();
    bool isRightPressed();
    
private:
    int i2c_fd;
    uint8_t device_address;
    
    bool writeRegister(uint8_t reg_base, uint8_t reg_addr, uint8_t* data, uint8_t length);
    bool readRegister(uint8_t reg_base, uint8_t reg_addr, uint8_t* data, uint8_t length);
};

#endif

