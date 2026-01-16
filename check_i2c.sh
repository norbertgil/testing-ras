#!/bin/bash
# Sprawdź status I2C i urządzenia

echo "=========================================="
echo "Diagnostyka I2C dla ANO Encoder"
echo "=========================================="
echo ""

echo "1. Sprawdzenie czy I2C jest włączone:"
if lsmod | grep -q i2c_bcm2835; then
    echo "   ✓ Moduł i2c_bcm2835 załadowany"
else
    echo "   ✗ Moduł i2c_bcm2835 NIE jest załadowany"
fi
echo ""

echo "2. Sprawdzenie prędkości I2C:"
if [ -f /sys/module/i2c_bcm2835/parameters/baudrate ]; then
    SPEED=$(cat /sys/module/i2c_bcm2835/parameters/baudrate)
    echo "   Prędkość: $SPEED Hz"
    if [ "$SPEED" = "100000" ]; then
        echo "   ⚠️  UWAGA: Domyślna prędkość 100 kHz - encoder potrzebuje 400 kHz!"
        echo "   Uruchom: ./setup_i2c_speed.sh"
    elif [ "$SPEED" = "400000" ]; then
        echo "   ✓ Optymalna prędkość 400 kHz"
    fi
else
    echo "   Nie można odczytać prędkości"
fi
echo ""

echo "3. Skanowanie urządzeń I2C:"
echo "   Szukam urządzeń na /dev/i2c-1..."
i2cdetect -y 1
echo ""

echo "4. Sprawdzenie czy urządzenie 0x49 odpowiada:"
if i2cdetect -y 1 | grep -q "49"; then
    echo "   ✓ Urządzenie znalezione na 0x49"
    echo ""
    echo "5. Test prostego odczytu (bez adresu rejestru):"
    i2cget -y 1 0x49 2>/dev/null && echo "   ✓ Podstawowy odczyt działa" || echo "   ✗ Podstawowy odczyt nie działa"
    echo ""
    echo "6. Test zapisu (soft reset na seesaw):"
    echo "   Próba soft reset: rejestr 0x00:0x7F wartość 0xFF"
    i2cset -y 1 0x49 0x00 0x7F 0xFF i 2>/dev/null && echo "   ✓ Reset wysłany" || echo "   ✗ Reset failed"
    sleep 1
    echo ""
    echo "7. Ponowne skanowanie po reset:"
    i2cdetect -y 1
else
    echo "   ✗ Urządzenie NIE odpowiada na 0x49"
    echo ""
    echo "   Sprawdź:"
    echo "   - Czy przewody są dobrze podłączone"
    echo "   - VIN -> Pin 1 (3.3V)"
    echo "   - GND -> Pin 9 (GND)"
    echo "   - SDA -> Pin 3 (GPIO 2)"
    echo "   - SCL -> Pin 5 (GPIO 3)"
fi

echo ""
echo "8. Test Pythona (czy działa):"
if [ -d "venv" ] && [ -f "test_ano_python.py" ]; then
    echo "   Uruchamiam krótki test Python..."
    source venv/bin/activate 2>/dev/null
    timeout 3 python test_ano_python.py 2>&1 | head -5
    deactivate 2>/dev/null
    echo "   (przerwano po 3 sekundach)"
else
    echo "   Brak środowiska Python do testu"
fi

echo ""
echo "=========================================="
echo "Koniec diagnostyki"
echo "=========================================="

