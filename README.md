# Adafruit ANO Rotary Encoder - Raspberry Pi C++ Project

This project provides a C++ library and example code for interfacing with the Adafruit ANO Rotary Navigation Encoder to I2C STEMMA QT Adapter on a Raspberry Pi.

## Hardware Setup

### Wiring

Connect the ANO Encoder to your Raspberry Pi as follows:

- **VIN** → Raspberry Pi 3.3V (Pin 1 or 17)
- **GND** → Raspberry Pi GND (Pin 6, 9, 14, 20, 25, 30, 34, or 39)
- **SDA** → Raspberry Pi SDA (GPIO 2, Pin 3)
- **SCL** → Raspberry Pi SCL (GPIO 3, Pin 5)

The default I2C address is **0x49**.

## Software Setup

### 1. Install Required Libraries

```bash
sudo apt-get update
sudo apt-get install libi2c-dev i2c-tools
```

### 2. Enable I2C on Raspberry Pi

```bash
sudo raspi-config
```

Navigate to: **Interface Options → I2C → Enable**

Reboot your Raspberry Pi after enabling I2C.

### 3. **IMPORTANT: Set I2C Speed to 400 kHz**

The ANO Encoder requires 400 kHz I2C speed (default is 100 kHz). This is **critical** for reliable encoder operation!

**Automated setup:**
```bash
chmod +x setup_i2c_speed.sh
./setup_i2c_speed.sh
```

**Manual setup:**
```bash
sudo nano /boot/firmware/config.txt
# Or on older Pi: sudo nano /boot/config.txt
```

Add this line:
```
dtparam=i2c_arm_baudrate=400000
```

Save and reboot:
```bash
sudo reboot
```

**Verify the speed:**
```bash
chmod +x check_i2c_speed.sh
./check_i2c_speed.sh
```

### 4. Verify I2C Connection

After connecting your encoder, verify it's detected:

```bash
i2cdetect -y 1
```

You should see a device at address 0x49 (or whichever address you configured).

## Building the Project

### Compile

```bash
make
```

### Run

```bash
sudo make run
```

Or run directly:

```bash
sudo ./ano_encoder_test
```

**Note:** Root privileges (sudo) are required to access I2C devices.

### Diagnostic Tool (Troubleshooting)

If you're having issues with button detection or encoder rotation:

```bash
make diagnostic
sudo make diagnostic
```

This runs a diagnostic tool that shows raw button states every second. It will help identify:
- Which buttons are actually being detected
- If the encoder position is changing
- Hardware connection issues

### Clean Build

```bash
make clean
```

## Features

- **I2C Communication**: Uses Linux I2C device interface (`/dev/i2c-1`)
- **Seesaw Protocol**: Implements the seesaw firmware communication protocol
- **Encoder Tracking**: Reads rotary encoder position and delta values
- **Button Support**: All 5 buttons (Select, Up, Down, Left, Right)
- **Product Verification**: Checks for correct firmware (product ID 5740)

## Project Structure

```
.
├── ano_encoder.h       # Header file with class definition
├── ano_encoder.cpp     # Implementation of the ANOEncoder class
├── main.cpp            # Example usage and test program
├── Makefile            # Build configuration
└── README.md           # This file
```

## API Reference

### Constructor

```cpp
ANOEncoder(uint8_t address = 0x49, const char* i2c_device = "/dev/i2c-1");
```

### Methods

- `bool begin()` - Initialize the encoder
- `uint32_t getVersion()` - Get firmware version
- `int32_t getEncoderPosition()` - Get absolute encoder position
- `int32_t getEncoderDelta()` - Get encoder change since last read
- `bool isSelectPressed()` - Check if center button is pressed
- `bool isUpPressed()` - Check if up button is pressed
- `bool isDownPressed()` - Check if down button is pressed
- `bool isLeftPressed()` - Check if left button is pressed
- `bool isRightPressed()` - Check if right button is pressed

## Custom I2C Address

If you've configured a different I2C address by cutting jumpers on the board:

```cpp
ANOEncoder encoder(0x4A); // A0 jumper cut
```

Available addresses with jumper configurations:
- 0x49 (default)
- 0x4A (A0 cut)
- 0x4B (A1 cut)
- 0x4C (A0 + A1 cut)

## Troubleshooting

### "Failed to open I2C device"
- Ensure I2C is enabled in raspi-config
- Check that `/dev/i2c-1` exists
- Run with sudo

### "Wrong firmware loaded"
- Verify the ANO encoder is properly connected
- Check I2C connections with `i2cdetect -y 1`
- Ensure you're using the correct device (product ID should be 5740)

### No response from buttons or encoder
- Check wiring connections
- Verify I2C address matches your configuration
- Test I2C communication with `i2cdetect` and `i2cget`

## License

This code is provided as-is for use with Adafruit hardware.

## References

- [Adafruit ANO Encoder Product Page](https://www.adafruit.com/product/5740)
- [Adafruit ANO Encoder Guide](https://learn.adafruit.com/adafruit-ano-rotary-encoder)

