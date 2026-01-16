#!/bin/bash
# Setup script for Python environment on Raspberry Pi
# Handles externally managed environments

echo "=========================================="
echo "ANO Encoder Python Environment Setup"
echo "=========================================="
echo ""

# Check if running on Raspberry Pi
if [ ! -f /proc/device-tree/model ] || ! grep -q "Raspberry Pi" /proc/device-tree/model 2>/dev/null; then
    echo "⚠️  Warning: This doesn't appear to be a Raspberry Pi"
    echo "   The script will continue, but hardware features may not work."
    echo ""
fi

# Check for required system packages
echo "Checking system dependencies..."
MISSING_DEPS=()

if ! command -v swig &> /dev/null; then
    MISSING_DEPS+=("swig")
fi

if ! dpkg -s python3-dev &> /dev/null; then
    MISSING_DEPS+=("python3-dev")
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    echo ""
    echo "⚠️  Missing system dependencies: ${MISSING_DEPS[*]}"
    echo ""
    echo "Please run first:"
    echo "  chmod +x install_system_deps.sh"
    echo "  ./install_system_deps.sh"
    echo ""
    exit 1
fi

echo "✓ System dependencies OK"
echo ""

# Create virtual environment
echo "Creating Python virtual environment..."
if [ -d "venv" ]; then
    echo "⚠️  Virtual environment already exists at ./venv"
    read -p "Do you want to recreate it? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rm -rf venv
        python3 -m venv venv
        echo "✓ Virtual environment recreated"
    else
        echo "Using existing virtual environment"
    fi
else
    python3 -m venv venv
    echo "✓ Virtual environment created"
fi

# Activate virtual environment
echo ""
echo "Activating virtual environment..."
source venv/bin/activate

# Upgrade pip
echo ""
echo "Upgrading pip..."
pip install --upgrade pip

# Install dependencies
echo ""
echo "Installing dependencies..."
pip install -r requirements.txt

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ Setup completed successfully!"
    echo "=========================================="
    echo ""
    echo "To use the Python script:"
    echo "  1. Activate the virtual environment:"
    echo "     source venv/bin/activate"
    echo ""
    echo "  2. Run the test script:"
    echo "     python test_ano_python.py"
    echo ""
    echo "  3. When done, deactivate:"
    echo "     deactivate"
    echo ""
else
    echo ""
    echo "❌ Installation failed!"
    echo "Please check the error messages above."
    exit 1
fi
