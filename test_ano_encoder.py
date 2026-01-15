#!/usr/bin/env python3
"""
ANO Rotary Encoder Test Script
Based on Adafruit's official example code
Reference: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ano-rotary-navigation-encoder-to-i2c-stemma-qt-adapter.pdf
"""

import board
import time
from adafruit_seesaw import seesaw, rotaryio, digitalio

# Button pin definitions (from Adafruit documentation)
SS_SWITCH_SELECT = 1
SS_SWITCH_UP = 2
SS_SWITCH_LEFT = 3
SS_SWITCH_DOWN = 4
SS_SWITCH_RIGHT = 5

def main():
    print("=" * 50)
    print("ANO Rotary Encoder Test")
    print("=" * 50)
    
    # Initialize I2C
    try:
        i2c = board.I2C()  # uses board.SCL and board.SDA
        print("✓ I2C initialized")
    except Exception as e:
        print(f"✗ Failed to initialize I2C: {e}")
        return
    
    # Initialize seesaw at default address 0x49
    try:
        ss = seesaw.Seesaw(i2c, addr=0x49)
        print("✓ Seesaw initialized at address 0x49")
    except Exception as e:
        print(f"✗ Failed to initialize seesaw: {e}")
        print("  Make sure the encoder is connected and I2C is enabled")
        return
    
    # Check firmware version and product ID
    try:
        version = ss.get_version()
        seesaw_product = (version >> 16) & 0xFFFF
        print(f"✓ Firmware version: 0x{version:08X}")
        print(f"✓ Product ID: {seesaw_product}")
        
        if seesaw_product != 5740:
            print(f"✗ WARNING: Expected product ID 5740, got {seesaw_product}")
            print("  Wrong firmware loaded?")
            return
        else:
            print("✓ Product ID verified (5740 - ANO Encoder)")
    except Exception as e:
        print(f"✗ Failed to read firmware version: {e}")
        return
    
    # Configure button pins as inputs with pullups
    print("\nConfiguring buttons...")
    try:
        ss.pin_mode(SS_SWITCH_SELECT, ss.INPUT_PULLUP)
        ss.pin_mode(SS_SWITCH_UP, ss.INPUT_PULLUP)
        ss.pin_mode(SS_SWITCH_LEFT, ss.INPUT_PULLUP)
        ss.pin_mode(SS_SWITCH_DOWN, ss.INPUT_PULLUP)
        ss.pin_mode(SS_SWITCH_RIGHT, ss.INPUT_PULLUP)
        print("✓ All buttons configured")
    except Exception as e:
        print(f"✗ Failed to configure buttons: {e}")
        return
    
    # Create digital IO objects for buttons
    select = digitalio.DigitalIO(ss, SS_SWITCH_SELECT)
    up = digitalio.DigitalIO(ss, SS_SWITCH_UP)
    left = digitalio.DigitalIO(ss, SS_SWITCH_LEFT)
    down = digitalio.DigitalIO(ss, SS_SWITCH_DOWN)
    right = digitalio.DigitalIO(ss, SS_SWITCH_RIGHT)
    
    # Button state tracking
    select_held = False
    up_held = False
    left_held = False
    down_held = False
    right_held = False
    
    # Create encoder object
    encoder = rotaryio.IncrementalEncoder(ss)
    last_position = encoder.position
    
    print(f"✓ Encoder initialized at position: {last_position}")
    print("\n" + "=" * 50)
    print("Testing started!")
    print("=" * 50)
    print("Rotate the encoder or press buttons to test.")
    print("Press Ctrl+C to exit.\n")
    
    # Main test loop
    try:
        while True:
            # Check encoder position
            position = encoder.position
            if position != last_position:
                delta = position - last_position
                direction = "clockwise" if delta > 0 else "counter-clockwise"
                print(f"🔄 Encoder position: {position:4d} (moved {direction}, delta: {delta:+d})")
                last_position = position
            
            # Check SELECT button
            if not select.value and not select_held:
                select_held = True
                print("🔵 SELECT button pressed")
            elif select.value and select_held:
                select_held = False
                print("⚪ SELECT button released")
            
            # Check UP button
            if not up.value and not up_held:
                up_held = True
                print("⬆️  UP button pressed")
            elif up.value and up_held:
                up_held = False
                print("⚪ UP button released")
            
            # Check DOWN button
            if not down.value and not down_held:
                down_held = True
                print("⬇️  DOWN button pressed")
            elif down.value and down_held:
                down_held = False
                print("⚪ DOWN button released")
            
            # Check LEFT button
            if not left.value and not left_held:
                left_held = True
                print("⬅️  LEFT button pressed")
            elif left.value and left_held:
                left_held = False
                print("⚪ LEFT button released")
            
            # Check RIGHT button
            if not right.value and not right_held:
                right_held = True
                print("➡️  RIGHT button pressed")
            elif right.value and right_held:
                right_held = False
                print("⚪ RIGHT button released")
            
            # Small delay to prevent overwhelming the I2C bus
            time.sleep(0.01)
            
    except KeyboardInterrupt:
        print("\n\n" + "=" * 50)
        print("Test stopped by user")
        print("=" * 50)
        print(f"Final encoder position: {position}")
        print("Test completed successfully!")

if __name__ == "__main__":
    main()

