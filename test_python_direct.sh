#!/bin/bash
# Test bezpośredniego uruchomienia Python

echo "=========================================="
echo "Test Python script"
echo "=========================================="
echo ""

if [ ! -f "venv/bin/python" ]; then
    echo "❌ Brak venv/bin/python"
    echo "Uruchom: ./setup_python_env_system_packages.sh"
    exit 1
fi

echo "✓ venv istnieje"
echo ""
echo "Uruchamiam encoder_reader.py..."
echo "Ctrl+C aby przerwać"
echo ""

venv/bin/python encoder_reader.py

