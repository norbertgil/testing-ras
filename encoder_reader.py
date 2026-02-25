#!/usr/bin/env python3
"""
Odczyt 3 encoderów + 1 przycisk:
1) ANO encoder przez Seesaw (0x49) - ENCODER + 5 przycisków
2-3) 2x rotary encoder przez MCP23017 (0x20):
     ENC2: A0/A1/A2, ENC3: A3/A4/A5
4) Przycisk BTN_EXT na A6
Wolne: A7, B0-B7
"""
import sys
import time

print("STARTING", flush=True)

try:
    import board
    from adafruit_seesaw import seesaw, rotaryio, digitalio
    from adafruit_mcp230xx.mcp23017 import MCP23017
    import digitalio as core_digitalio
    print("IMPORTS_OK", flush=True)
except ImportError as e:
    print(f"ERROR:Import failed: {e}", flush=True)
    sys.exit(1)

# pin_a, pin_b, pin_btn (MCP23017 pin numbers: 0-7 = GPA0-GPA7, 8-15 = GPB0-GPB7)
MCP_ENCODERS = [
    {"name": "ENC2", "pin_a": 0, "pin_b": 1, "pin_btn": 2},  # A0, A1, A2
    {"name": "ENC3", "pin_a": 3, "pin_b": 4, "pin_btn": 5},  # A3, A4, A5
]

MCP_BUTTONS = [
    {"name": "BTN_EXT", "pin": 6},  # A6
]

def setup_mcp_pin(mcp, pin_num):
    pin = mcp.get_pin(pin_num)
    pin.direction = core_digitalio.Direction.INPUT
    pin.pull = core_digitalio.Pull.UP
    return pin

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

    for pin in (1, 2, 3, 4, 5):
        ss.pin_mode(pin, ss.INPUT_PULLUP)

    select = digitalio.DigitalIO(ss, 1)
    up = digitalio.DigitalIO(ss, 2)
    left = digitalio.DigitalIO(ss, 3)
    down = digitalio.DigitalIO(ss, 4)
    right = digitalio.DigitalIO(ss, 5)

    encoder = rotaryio.IncrementalEncoder(ss)
    print("ANO_ENCODER_OK", flush=True)

    # --- MCP23017 Expander (0x20) - 4 encodery ---
    print("INIT_MCP23017", flush=True)
    mcp = MCP23017(i2c, address=0x20)

    enc_states = []
    for cfg in MCP_ENCODERS:
        pin_a = setup_mcp_pin(mcp, cfg["pin_a"])
        pin_b = setup_mcp_pin(mcp, cfg["pin_b"])
        pin_btn = setup_mcp_pin(mcp, cfg["pin_btn"])
        enc_states.append({
            "name": cfg["name"],
            "pin_a": pin_a,
            "pin_b": pin_b,
            "pin_btn": pin_btn,
            "position": 0,
            "last_position": 0,
            "last_a": pin_a.value,
            "btn_state": False,
        })
        print(f"{cfg['name']}_OK", flush=True)

    ext_btn_states = []
    for cfg in MCP_BUTTONS:
        pin = setup_mcp_pin(mcp, cfg["pin"])
        ext_btn_states.append({
            "name": cfg["name"],
            "pin": pin,
            "state": False,
        })
        print(f"{cfg['name']}_OK", flush=True)

    print("MCP23017_OK", flush=True)
    print("READY", flush=True)

    last_position = None
    ano_buttons = [
        (select, "SELECT"),
        (up, "UP"),
        (down, "DOWN"),
        (left, "LEFT"),
        (right, "RIGHT"),
    ]
    ano_btn_states = [False] * 5

    while True:
        # --- ANO Encoder ---
        position = encoder.position
        if position != last_position:
            print(f"ENCODER:{position}", flush=True)
            last_position = position

        for i, (button, name) in enumerate(ano_buttons):
            pressed = not button.value
            if pressed and not ano_btn_states[i]:
                print(f"BUTTON:{name}:PRESSED", flush=True)
                ano_btn_states[i] = True
            elif not pressed and ano_btn_states[i]:
                print(f"BUTTON:{name}:RELEASED", flush=True)
                ano_btn_states[i] = False

        # --- MCP23017 Encodery (software decoding) ---
        for es in enc_states:
            a_val = es["pin_a"].value
            b_val = es["pin_b"].value

            if a_val != es["last_a"]:
                if a_val != b_val:
                    es["position"] += 1
                else:
                    es["position"] -= 1

                if es["position"] != es["last_position"]:
                    print(f"{es['name']}:{es['position']}", flush=True)
                    es["last_position"] = es["position"]

            es["last_a"] = a_val

            btn_pressed = not es["pin_btn"].value
            if btn_pressed and not es["btn_state"]:
                print(f"BUTTON:{es['name']}:PRESSED", flush=True)
                es["btn_state"] = True
            elif not btn_pressed and es["btn_state"]:
                print(f"BUTTON:{es['name']}:RELEASED", flush=True)
                es["btn_state"] = False

        # --- MCP23017 standalone buttons ---
        for bs in ext_btn_states:
            pressed = not bs["pin"].value
            if pressed and not bs["state"]:
                print(f"BUTTON:{bs['name']}:PRESSED", flush=True)
                bs["state"] = True
            elif not pressed and bs["state"]:
                print(f"BUTTON:{bs['name']}:RELEASED", flush=True)
                bs["state"] = False

except KeyboardInterrupt:
    pass
except Exception as e:
    print(f"ERROR:{e}", flush=True)
    sys.exit(1)

