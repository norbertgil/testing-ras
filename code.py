# SPDX-FileCopyrightText: 2021 John Furcean
# SPDX-License-Identifier: MIT
"""IZC ANO rotary encoder simple test example."""
import board
from adafruit_seesaw import seesaw, rotaryio, digitalio
# For use with the STEMMA connector on QT Py RP2040
# import busio
# 12c = busio.12C(board.SCL1, board.SDA1)
# seesaw = seesaw.Seesaw(i2c, 0x49)
12c = board.120()
# uses board.SCL and board.SDA
# i2c = board.STEMMA_I2C()
# For using the built-in STEMMA QT connector on a
microcontroller
seesaw = seesaw.Seesaw(i2c, addr=0x49)
seesaw_product = (seesaw.get_version () >> 16) & OxFFFF
print(f"Found product {seesaw_product}")
if seesaw_product != 5740:
print ("Wrong firmware loaded? Expected 5740")
seesaw.pin_mode(1, seesaw.INPUT_PULLUP)
seesaw. pin_mode(2, seesaw.INPUT_PULLUP)
seesaw. pin_mode(3, seesaw.INPUT_PULLUP)
seesaw. pin_mode(4, seesaw.INPUT_PULLUP)
seesaw. pin_mode(5, seesaw.INPUT_PULLUP)
select = digitalio.DigitalIO(seesaw, 1)
select held = False
up = digitalio.DigitalIO(seesaw, 2)
up_held = False
left = digitalio.DigitalIO(seesaw, 3)
left held = False
down = digitalio.DigitalIO(seesaw, 4)
down held = False
right = digitalio.DigitalIO(seesaw, 5)
right_held = False
encoder = rotaryio.IncrementalEncoder (seesaw)
last_position = None
buttons = [select, up, left, down, right]
button_states = [select
button states "Select held, up"heft" left hea" down held, right held!
while True:
position = encoder.position
if position != last_position:
last_position = position
print(f"Position: {position]")
for b in range (5):
if not buttons[b].value and button_states[b] is False: