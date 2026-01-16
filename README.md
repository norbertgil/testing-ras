# Adafruit ANO Rotary Encoder - Raspberry Pi C++ + Python

This project provides a C++ application that uses Python backend to interface with the Adafruit ANO Rotary Navigation Encoder to I2C STEMMA QT Adapter on a Raspberry Pi.

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

## Quick Start

### 1. Install System Dependencies

```bash
chmod +x install_system_deps.sh
./install_system_deps.sh
```

### 2. Setup Python Environment

```bash
chmod +x setup_python_env_system_packages.sh
./setup_python_env_system_packages.sh
```

### 3. Build and Run

```bash
make
make run
```

The C++ application will start and display encoder position and button presses in real-time.

### Test Python Backend Directly (Optional)

```bash
chmod +x test_python_direct.sh
./test_python_direct.sh
```

### Clean Build

```bash
make clean
```

## Architecture

- **C++ Application**: Main program that displays encoder data
- **Python Backend**: Uses Adafruit libraries to communicate with encoder via I2C
- **Hybrid Approach**: C++ for UI/logic, Python for hardware communication

## Features

- ✅ Rotary encoder position tracking with change delta
- ✅ All 5 buttons supported (Select, Up, Down, Left, Right)
- ✅ Real-time event display
- ✅ Automatic I2C communication via Python
- ✅ Clean separation between C++ logic and Python hardware layer

## Project Structure

```
.
├── encoder_app.cpp                         # C++ application (main program)
├── encoder_reader.py                       # Python backend (reads encoder via I2C)
├── Makefile                                # Build configuration
├── install_system_deps.sh                  # Install system packages
├── setup_python_env_system_packages.sh     # Setup Python environment
├── setup_i2c_speed.sh                      # Configure I2C to 400 kHz
├── check_i2c_speed.sh                      # Check I2C configuration
├── test_python_direct.sh                   # Test Python backend
├── requirements.txt                        # Python dependencies
└── README.md                               # This file
```

## How It Works

1. **C++ Program** (`encoder_app.cpp`) starts and launches Python backend
2. **Python Script** (`encoder_reader.py`) initializes encoder and continuously reads:
   - Encoder position changes
   - Button press/release events
3. **Communication**: Python sends data to C++ via stdout in simple format:
   - `ENCODER:123` - encoder position
   - `BUTTON:SELECT:PRESSED` - button event
4. **C++ Display**: Formats and displays data with emojis and formatting

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

