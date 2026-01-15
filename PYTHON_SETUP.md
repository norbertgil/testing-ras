# Python Setup Guide for ANO Rotary Encoder

This guide will help you set up and test the ANO Rotary Encoder using Python on your Raspberry Pi.

## Prerequisites

1. **Enable I2C on Raspberry Pi:**
   ```bash
   sudo raspi-config
   ```
   - Navigate to: **Interface Options → I2C → Enable**
   - Reboot after enabling: `sudo reboot`

2. **Verify I2C is working:**
   ```bash
   # Install i2c-tools if not already installed
   sudo apt-get update
   sudo apt-get install -y python3-pip i2c-tools
   
   # Check if the encoder is detected (should show 0x49)
   i2cdetect -y 1
   ```

## Installation

### 1. Install Python Dependencies

#### Option A: Using Virtual Environment (Recommended for Externally Managed Environments)

If you get an "externally managed environment" error, use a virtual environment:

```bash
# Create virtual environment
python3 -m venv venv

# Activate virtual environment
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt
```

**Or use the automated setup script:**

```bash
chmod +x setup_python_env.sh
./setup_python_env.sh
```

#### Option B: System-wide Installation (Older Raspberry Pi OS)

```bash
# Install required Python packages
pip3 install -r requirements.txt

# Or install manually:
pip3 install adafruit-circuitpython-seesaw adafruit-blinka
```

#### Option C: Override External Management (Not Recommended)

```bash
# Use --break-system-packages flag (only if you understand the implications)
pip3 install --break-system-packages -r requirements.txt
```

### 2. Verify Installation

```bash
python3 -c "import board; import adafruit_seesaw; print('Installation successful!')"
```

## Hardware Connections

Connect the ANO Encoder to your Raspberry Pi:

| ANO Encoder Pin | Raspberry Pi Pin | GPIO | Physical Pin |
|----------------|------------------|------|--------------|
| VIN            | 3.3V Power       | -    | Pin 1 or 17  |
| GND            | Ground           | -    | Pin 6, 9, 14, 20, 25, 30, 34, or 39 |
| SDA            | I2C Data         | GPIO 2 | Pin 3      |
| SCL            | I2C Clock        | GPIO 3 | Pin 5      |

**Default I2C Address:** 0x49

## Running the Test Script

### If Using Virtual Environment

```bash
# Activate the virtual environment first
source venv/bin/activate

# Run the test script
python test_ano_encoder.py

# When done, deactivate
deactivate
```

### If Installed System-wide

```bash
python3 test_ano_encoder.py
```

### Make it Executable (Optional)

```bash
chmod +x test_ano_encoder.py
./test_ano_encoder.py
```

## What the Test Script Does

The test script will:

1. ✓ Initialize I2C communication
2. ✓ Connect to the ANO encoder at address 0x49
3. ✓ Verify firmware version and product ID (should be 5740)
4. ✓ Configure all 5 buttons (Select, Up, Down, Left, Right)
5. ✓ Monitor encoder rotation and button presses in real-time
6. ✓ Display all events to the console

## Expected Output

When you run the script, you should see:

```
==================================================
ANO Rotary Encoder Test
==================================================
✓ I2C initialized
✓ Seesaw initialized at address 0x49
✓ Firmware version: 0x164C0001
✓ Product ID: 5740
✓ Product ID verified (5740 - ANO Encoder)

Configuring buttons...
✓ All buttons configured
✓ Encoder initialized at position: 0

==================================================
Testing started!
==================================================
Rotate the encoder or press buttons to test.
Press Ctrl+C to exit.

🔄 Encoder position:    1 (moved clockwise, delta: +1)
🔄 Encoder position:    2 (moved clockwise, delta: +1)
🔵 SELECT button pressed
⚪ SELECT button released
⬆️  UP button pressed
⚪ UP button released
```

## Troubleshooting

### "Failed to initialize I2C"
- Make sure I2C is enabled in `raspi-config`
- Reboot after enabling I2C
- Check wiring connections

### "Failed to initialize seesaw"
- Verify the encoder is properly connected
- Check I2C address with `i2cdetect -y 1` (should show 0x49)
- Ensure VIN is connected to 3.3V and GND to ground

### "Expected product ID 5740"
- Wrong device connected
- Incompatible firmware version
- Check that you're using the ANO Rotary Encoder

### ImportError: No module named 'board' or 'adafruit_seesaw'
- If using a virtual environment, make sure it's activated: `source venv/bin/activate`
- Install dependencies: `pip install -r requirements.txt`
- For externally managed environments, use the setup script: `./setup_python_env.sh`

### Permission Denied
- Make sure you're running as a user with I2C permissions
- Add your user to the i2c group: `sudo usermod -aG i2c $USER`
- Log out and back in for changes to take effect

## Testing Checklist

Use this checklist to verify all functionality:

- [ ] Script initializes without errors
- [ ] Product ID 5740 is detected
- [ ] Encoder rotates clockwise (position increases)
- [ ] Encoder rotates counter-clockwise (position decreases)
- [ ] SELECT (center) button press/release detected
- [ ] UP button press/release detected
- [ ] DOWN button press/release detected
- [ ] LEFT button press/release detected
- [ ] RIGHT button press/release detected

## Additional Configuration

### Custom I2C Address

If you've modified the I2C address by cutting jumpers, edit the script:

```python
# Change this line in test_ano_encoder.py:
ss = seesaw.Seesaw(i2c, addr=0x49)  # Change 0x49 to your address
```

### Available Addresses

| Address | A0 | A1 | A2 | A3 |
|---------|----|----|----|----|
| 0x49    | Closed | Closed | Closed | Closed |
| 0x4A    | Cut | Closed | Closed | Closed |
| 0x4B    | Closed | Cut | Closed | Closed |
| 0x4C    | Cut | Cut | Closed | Closed |
| ... and more (see main README.md) |

## References

- [Adafruit ANO Encoder Guide PDF](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ano-rotary-navigation-encoder-to-i2c-stemma-qt-adapter.pdf)
- [Adafruit Learn Guide](https://learn.adafruit.com/adafruit-ano-rotary-encoder)
- [Adafruit Seesaw Python Library](https://github.com/adafruit/Adafruit_CircuitPython_seesaw)

## Support

If you continue to have issues:
1. Check the [Adafruit Forums](https://forums.adafruit.com/)
2. Review the official [Adafruit ANO Encoder guide](https://learn.adafruit.com/adafruit-ano-rotary-encoder)
3. Verify hardware connections and solder joints

