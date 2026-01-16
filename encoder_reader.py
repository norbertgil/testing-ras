#!/usr/bin/env python3
"""
Prosty skrypt do odczytu ANO encodera
Drukuje dane w formacie: ENCODER:position BUTTON:name
"""
import sys
import board
from adafruit_seesaw import seesaw, rotaryio, digitalio

try:
    # Inicjalizacja
    i2c = board.I2C()
    ss = seesaw.Seesaw(i2c, addr=0x49)
    
    # Sprawdź produkt
    product_id = (ss.get_version() >> 16) & 0xFFFF
    if product_id != 5740:
        print(f"ERROR:Wrong product {product_id}", flush=True)
        sys.exit(1)
    
    # Skonfiguruj przyciski
    ss.pin_mode(1, ss.INPUT_PULLUP)
    ss.pin_mode(2, ss.INPUT_PULLUP)
    ss.pin_mode(3, ss.INPUT_PULLUP)
    ss.pin_mode(4, ss.INPUT_PULLUP)
    ss.pin_mode(5, ss.INPUT_PULLUP)
    
    select = digitalio.DigitalIO(ss, 1)
    up = digitalio.DigitalIO(ss, 2)
    left = digitalio.DigitalIO(ss, 3)
    down = digitalio.DigitalIO(ss, 4)
    right = digitalio.DigitalIO(ss, 5)
    
    # Encoder
    encoder = rotaryio.IncrementalEncoder(ss)
    
    print("READY", flush=True)
    
    last_position = None
    buttons = [
        (select, "SELECT"),
        (up, "UP"),
        (down, "DOWN"),
        (left, "LEFT"),
        (right, "RIGHT")
    ]
    button_states = [False] * 5
    
    while True:
        # Odczytaj pozycję
        position = encoder.position
        if position != last_position:
            print(f"ENCODER:{position}", flush=True)
            last_position = position
        
        # Sprawdź przyciski
        for i, (button, name) in enumerate(buttons):
            pressed = not button.value
            if pressed and not button_states[i]:
                print(f"BUTTON:{name}:PRESSED", flush=True)
                button_states[i] = True
            elif not pressed and button_states[i]:
                print(f"BUTTON:{name}:RELEASED", flush=True)
                button_states[i] = False

except KeyboardInterrupt:
    pass
except Exception as e:
    print(f"ERROR:{e}", flush=True)
    sys.exit(1)

