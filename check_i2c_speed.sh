#!/bin/bash
# Check current I2C bus speed

echo "=========================================="
echo "Current I2C Configuration"
echo "=========================================="
echo ""

# Check config file setting
CONFIG_FILE="/boot/firmware/config.txt"
FALLBACK_CONFIG="/boot/config.txt"

if [ -f "$CONFIG_FILE" ]; then
    TARGET_CONFIG="$CONFIG_FILE"
elif [ -f "$FALLBACK_CONFIG" ]; then
    TARGET_CONFIG="$FALLBACK_CONFIG"
fi

echo "Config file: $TARGET_CONFIG"
if grep -q "i2c_arm_baudrate" "$TARGET_CONFIG" 2>/dev/null; then
    echo "Setting in config.txt:"
    grep "i2c_arm_baudrate" "$TARGET_CONFIG"
else
    echo "No custom I2C baudrate in config.txt (default: 100 kHz)"
fi

echo ""
echo "Current I2C bus speed (from kernel):"
if [ -f /sys/module/i2c_bcm2835/parameters/baudrate ]; then
    SPEED=$(cat /sys/module/i2c_bcm2835/parameters/baudrate)
    echo "  $SPEED Hz"
    
    if [ "$SPEED" = "100000" ]; then
        echo "  ⚠️  WARNING: Default speed (100 kHz) - ANO Encoder needs 400 kHz!"
    elif [ "$SPEED" = "400000" ]; then
        echo "  ✓ Optimal speed for ANO Encoder (400 kHz)"
    fi
else
    echo "  Unable to read current speed"
fi

echo ""
echo "To change I2C speed to 400 kHz:"
echo "  ./setup_i2c_speed.sh"
echo ""

