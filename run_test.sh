#!/bin/bash
# Quick test runner script that handles virtual environment

echo "Starting ANO Encoder Test..."
echo ""

# Check if virtual environment exists
if [ -d "venv" ]; then
    echo "Activating virtual environment..."
    source venv/bin/activate
    python test_ano_encoder.py
    EXIT_CODE=$?
    deactivate
    exit $EXIT_CODE
else
    echo "No virtual environment found. Running with system Python..."
    python3 test_ano_encoder.py
fi

