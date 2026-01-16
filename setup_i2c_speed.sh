#!/bin/bash
# Script to set I2C bus speed to 400 kHz for ANO Encoder

echo "=========================================="
echo "I2C Speed Configuration for ANO Encoder"
echo "=========================================="
echo ""

CONFIG_FILE="/boot/firmware/config.txt"
FALLBACK_CONFIG="/boot/config.txt"

# Determine which config file exists
if [ -f "$CONFIG_FILE" ]; then
    TARGET_CONFIG="$CONFIG_FILE"
elif [ -f "$FALLBACK_CONFIG" ]; then
    TARGET_CONFIG="$FALLBACK_CONFIG"
else
    echo "❌ Could not find config.txt"
    echo "   Tried: $CONFIG_FILE"
    echo "   Tried: $FALLBACK_CONFIG"
    exit 1
fi

echo "Using config file: $TARGET_CONFIG"
echo ""

# Check current setting
if grep -q "^dtparam=i2c_arm=on,i2c_arm_baudrate=" "$TARGET_CONFIG"; then
    CURRENT=$(grep "^dtparam=i2c_arm=on,i2c_arm_baudrate=" "$TARGET_CONFIG")
    echo "Current I2C setting found: $CURRENT"
elif grep -q "^dtparam=i2c_arm_baudrate=" "$TARGET_CONFIG"; then
    CURRENT=$(grep "^dtparam=i2c_arm_baudrate=" "$TARGET_CONFIG")
    echo "Current I2C baudrate setting: $CURRENT"
else
    echo "No custom I2C baudrate setting found (using default 100 kHz)"
fi

echo ""
echo "The ANO Encoder requires 400 kHz I2C speed for best performance."
echo "This script will add/update the I2C baudrate setting."
echo ""
read -p "Do you want to set I2C speed to 400 kHz? (y/N): " -n 1 -r
echo

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Cancelled."
    exit 0
fi

echo ""
echo "Backing up config file..."
sudo cp "$TARGET_CONFIG" "${TARGET_CONFIG}.backup.$(date +%Y%m%d_%H%M%S)"

# Remove old settings if they exist
sudo sed -i '/^dtparam=i2c_arm_baudrate=/d' "$TARGET_CONFIG"

# Add new setting
echo ""
echo "Adding I2C speed setting..."
echo "dtparam=i2c_arm_baudrate=400000" | sudo tee -a "$TARGET_CONFIG" > /dev/null

echo ""
echo "=========================================="
echo "✓ I2C speed configured to 400 kHz"
echo "=========================================="
echo ""
echo "IMPORTANT: You must REBOOT for changes to take effect!"
echo ""
read -p "Reboot now? (y/N): " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Rebooting..."
    sudo reboot
else
    echo ""
    echo "Remember to reboot later with: sudo reboot"
fi

