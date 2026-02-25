#!/usr/bin/env python3
"""
Odczyt dwóch encoderów:
1) ANO encoder przez Seesaw (0x49)
2) Zwykły rotary encoder przez MCP23017 expander (0x20) - A0/A1 encoder, A2 button
"""
import sys
import time

print("STARTING", flush=True)

try:
    import board
    import busio
    from adafruit_seesaw import seesaw, rotaryio, digitalio
    from adafruit_mcp230xx.mcp23017 import MCP23017
    import digitalio as core_digitalio
    print("IMPORTS_OK", flush=True)
except ImportError as e:
    print(f"ERROR:Import failed: {e}", flush=True)
    sys.exit(1)

try:
    print("INIT_I2C", flush=True)
    i2c = board.I2C()

    # --- ANO Encoder (Seesaw 0x49) ---
    print("INIT_SEESAW", flush=True)
    ss = seesaw.Seesaw(i2c, addr=0x49)
    print("SEESAW_OK", flush=True)

    product_id = (ss.get_version() >> 16) & 0xFFFF
    if product_id != 5740:
        print(f"ERROR:Wrong product {product_id}", flush=True)
        sys.exit(1)

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

    encoder = rotaryio.IncrementalEncoder(ss)
    print("ANO_ENCODER_OK", flush=True)

    # --- MCP23017 Expander (0x20) - drugi encoder ---
    print("INIT_MCP23017", flush=True)
    mcp = MCP23017(i2c, address=0x20)

    enc2_a = mcp.get_pin(0)  # GPA0
    enc2_b = mcp.get_pin(1)  # GPA1
    enc2_btn = mcp.get_pin(2)  # GPA2

    enc2_a.direction = core_digitalio.Direction.INPUT
    enc2_a.pull = core_digitalio.Pull.UP
    enc2_b.direction = core_digitalio.Direction.INPUT
    enc2_b.pull = core_digitalio.Pull.UP
    enc2_btn.direction = core_digitalio.Direction.INPUT
    enc2_btn.pull = core_digitalio.Pull.UP

    print("MCP23017_OK", flush=True)

    # Software rotary decoder state
    enc2_position = 0
    enc2_last_a = enc2_a.value
    enc2_last_b = enc2_b.value

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
    enc2_last_position = 0
    enc2_btn_state = False

    while True:
        # --- ANO Encoder ---
        position = encoder.position
        if position != last_position:
            print(f"ENCODER:{position}", flush=True)
            last_position = position

        for i, (button, name) in enumerate(buttons):
            pressed = not button.value
            if pressed and not button_states[i]:
                print(f"BUTTON:{name}:PRESSED", flush=True)
                button_states[i] = True
            elif not pressed and button_states[i]:
                print(f"BUTTON:{name}:RELEASED", flush=True)
                button_states[i] = False

        # --- MCP23017 Encoder (software decoding) ---
        a_val = enc2_a.value
        b_val = enc2_b.value

        if a_val != enc2_last_a:
            if a_val != b_val:
                enc2_position += 1
            else:
                enc2_position -= 1

            if enc2_position != enc2_last_position:
                print(f"ENCODER2:{enc2_position}", flush=True)
                enc2_last_position = enc2_position

        enc2_last_a = a_val
        enc2_last_b = b_val

        btn_pressed = not enc2_btn.value
        if btn_pressed and not enc2_btn_state:
            print("BUTTON:ENCODER2:PRESSED", flush=True)
            enc2_btn_state = True
        elif not btn_pressed and enc2_btn_state:
            print("BUTTON:ENCODER2:RELEASED", flush=True)
            enc2_btn_state = False

except KeyboardInterrupt:
    pass
except Exception as e:
    print(f"ERROR:{e}", flush=True)
    sys.exit(1)

