#!/bin/bash
# Install system dependencies required for Adafruit libraries

echo "=========================================="
echo "Installing System Dependencies"
echo "=========================================="
echo ""

echo "This will install:"
echo "  - python3-dev (Python development headers)"
echo "  - python3-pip (Python package installer)"
echo "  - swig (Simplified Wrapper Interface Generator)"
echo "  - i2c-tools (I2C utilities)"
echo ""

read -p "Continue? (y/N): " -n 1 -r
echo

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Cancelled."
    exit 0
fi

echo ""
echo "Updating package list..."
sudo apt-get update

echo ""
echo "Installing dependencies..."
sudo apt-get install -y \
    python3-dev \
    python3-pip \
    swig \
    i2c-tools \
    libi2c-dev

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ System dependencies installed!"
    echo "=========================================="
    echo ""
    echo "Next steps:"
    echo "  1. Set I2C to 400 kHz: ./setup_i2c_speed.sh"
    echo "  2. Setup Python env: ./setup_python_env.sh"
    echo "  3. Test: source venv/bin/activate && python test_ano_python.py"
else
    echo ""
    echo "❌ Installation failed!"
    exit 1
fi

