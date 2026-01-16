# Python Setup Guide for ANO Rotary Encoder

This guide will help you set up and test the ANO Rotary Encoder using the official Adafruit Python example code.

**Reference:** [Adafruit ANO Encoder Guide (PDF)](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ano-rotary-navigation-encoder-to-i2c-stemma-qt-adapter.pdf)

## Prerequisites

### 1. Install System Dependencies

First, install required system packages:

```bash
chmod +x install_system_deps.sh
./install_system_deps.sh
```

This installs:
- `python3-dev` - Python development headers
- `swig` - Required for building native extensions
- `i2c-tools` - I2C utilities
- `libi2c-dev` - I2C development library

### 2. Enable I2C on Raspberry Pi

```bash
sudo raspi-config
```

- Navigate to: **Interface Options → I2C → Enable**
- Reboot after enabling: `sudo reboot`

### 3. **CRITICAL: Set I2C Speed to 400 kHz**

The ANO Encoder requires 400 kHz (default is 100 kHz):

```bash
chmod +x setup_i2c_speed.sh
./setup_i2c_speed.sh
sudo reboot
```

### 4. Verify I2C Connection

```bash
# Install i2c-tools if needed
sudo apt-get update
sudo apt-get install -y python3-pip i2c-tools

# Check if encoder is detected (should show 0x49)
i2cdetect -y 1
```

## Python Installation

### Option A: Using Virtual Environment (Recommended)

For externally managed Python environments (Raspberry Pi OS Bookworm+):

```bash
# Automated setup
chmod +x setup_python_env.sh
./setup_python_env.sh

# Activate virtual environment
source venv/bin/activate

# Run the test
python test_ano_python.py

# When done
deactivate
```

### Option B: Manual Virtual Environment Setup

```bash
# Create virtual environment
python3 -m venv venv

# Activate it
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Run the test
python test_ano_python.py
```

### Option C: System-wide Installation (Older Pi OS)

```bash
pip3 install -r requirements.txt
python3 test_ano_python.py
```

### Option D: Override External Management (Not Recommended)

```bash
pip3 install --break-system-packages -r requirements.txt
python3 test_ano_python.py
```

## Hardware Connections

| ANO Encoder Pin | Raspberry Pi Pin | GPIO     | Physical Pin |
|-----------------|------------------|----------|--------------|
| VIN             | 3.3V Power       | -        | Pin 1 or 17  |
| GND             | Ground           | -        | Pin 6, 9, etc|
| SDA             | I2C Data         | GPIO 2   | Pin 3        |
| SCL             | I2C Clock        | GPIO 3   | Pin 5        |

**Default I2C Address:** 0x49

## What the Python Script Does

The official Adafruit example (`test_ano_python.py`):

1. ✓ Connects to encoder at address 0x49
2. ✓ Verifies product ID (should be 5740)
3. ✓ Configures all 5 buttons
4. ✓ Monitors encoder rotation
5. ✓ Detects button press/release events

## Expected Output

```
Found product 5740
Position: 1
Position: 2
Position: 3
Select button pressed
Select button released
Up button pressed
Up button released
```

## Troubleshooting

### "Failed to initialize I2C"
- Enable I2C: `sudo raspi-config`
- Reboot after enabling
- Check wiring

### "Wrong firmware loaded? Expected 5740"
- Wrong device or incompatible firmware
- Verify with: `i2cdetect -y 1`

### Encoder not responding
- **Set I2C to 400 kHz** (most common issue!)
- Check: `./check_i2c_speed.sh`
- Fix: `./setup_i2c_speed.sh` and reboot

### ImportError: No module named 'board' or 'adafruit_seesaw'
- Activate virtual environment: `source venv/bin/activate`
- Install dependencies: `pip install -r requirements.txt`

### "externally managed environment" error
- Use virtual environment (see Option A above)
- Or run setup script: `./setup_python_env.sh`

## Quick Reference

```bash
# One-time setup
chmod +x install_system_deps.sh setup_python_env.sh setup_i2c_speed.sh check_i2c_speed.sh
./install_system_deps.sh  # Install system packages (swig, python3-dev, etc)
./setup_i2c_speed.sh      # Set I2C to 400 kHz, then reboot
./setup_python_env.sh     # Install Python packages

# Every time you want to test
source venv/bin/activate
python test_ano_python.py
# Press Ctrl+C to exit
deactivate
```

## References

- [Adafruit ANO Encoder Guide (PDF)](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ano-rotary-navigation-encoder-to-i2c-stemma-qt-adapter.pdf)
- [Adafruit Learn Guide](https://learn.adafruit.com/adafruit-ano-rotary-encoder)
- [Adafruit Seesaw Python Library](https://github.com/adafruit/Adafruit_CircuitPython_seesaw)
