#!/bin/bash
# Alternative setup using system packages (avoids building lgpio)

echo "=========================================="
echo "Python Setup Using System Packages"
echo "=========================================="
echo ""

echo "This approach uses system-installed Python packages"
echo "to avoid building native extensions."
echo ""

# Install system packages
echo "Installing system packages..."
sudo apt-get update
sudo apt-get install -y \
    python3-dev \
    python3-pip \
    swig \
    i2c-tools \
    libi2c-dev \
    python3-lgpio

# Create virtual environment with system packages access
echo ""
echo "Creating virtual environment with system packages..."
python3 -m venv --system-site-packages venv

echo ""
echo "Activating virtual environment..."
source venv/bin/activate

echo ""
echo "Installing Adafruit packages..."
pip install adafruit-blinka adafruit-circuitpython-seesaw

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ Setup completed successfully!"
    echo "=========================================="
    echo ""
    echo "To test:"
    echo "  source venv/bin/activate"
    echo "  python test_ano_python.py"
    echo ""
else
    echo ""
    echo "❌ Installation failed!"
    exit 1
fi

