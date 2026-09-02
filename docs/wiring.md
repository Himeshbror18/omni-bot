# Wiring — WeMos D1 R32 + 2× TB6612FNG

This is the **exact wiring reference for the current Omni-Bot firmware**. The controller is the **WeMos D1 R32 (ESP32)**.

For the full schematic and signal diagram, see [`SCHEMATICS.md`](SCHEMATICS.md).

## Controller and logic

- **WeMos D1 R32 3V3** → both TB6612FNG `VCC` logic inputs
- **WeMos D1 R32 GND** → both driver `GND`
- **GPIO32** → Driver A `STBY`
- **GPIO33** → Driver B `STBY`

## Driver A — Front Left + Front Right

| D1 R32 | TB6612FNG A | Purpose |
|---|---|---|
| GPIO13 | PWMA | FL PWM |
| GPIO18 | AIN1 | FL direction |
| GPIO19 | AIN2 | FL direction |
| GPIO14 | PWMB | FR PWM |
| GPIO21 | BIN1 | FR direction |
| GPIO22 | BIN2 | FR direction |
| GPIO32 | STBY | Driver enable |
| 3V3 | VCC | Logic supply |
| GND | GND | Common ground |

- `A01/A02` → Front Left motor
- `B01/B02` → Front Right motor

## Driver B — Rear Left + Rear Right

| D1 R32 | TB6612FNG B | Purpose |
|---|---|---|
| GPIO16 | PWMA | RL PWM |
| GPIO23 | AIN1 | RL direction |
| GPIO25 | AIN2 | RL direction |
| GPIO17 | PWMB | RR PWM |
| GPIO26 | BIN1 | RR direction |
| GPIO27 | BIN2 | RR direction |
| GPIO33 | STBY | Driver enable |
| 3V3 | VCC | Logic supply |
| GND | GND | Common ground |

- `A01/A02` → Rear Left motor
- `B01/B02` → Rear Right motor

## Motor power

- Motor battery `+` → both TB6612FNG `VM` inputs
- Motor battery `−` → common `GND`
- Never power the motors from the D1 R32 3.3 V rail.
- Never feed the motor-battery voltage into an ESP32 GPIO.
- Check the exact TB6612FNG breakout board's regulator/jumper arrangement before power-up.

## First test

1. Keep all four wheels off the ground.
2. Verify every wire against the tables above.
3. Power the controller and drivers.
4. Connect to `Omni-Bot` Wi-Fi.
5. Test each motor individually at low speed.
6. Confirm physical direction.
7. Adjust the firmware inversion flags or motor polarity if needed.
8. Test forward, reverse, strafe and rotation.

The firmware currently in `firmware/src/main.cpp` uses the same pin map and motor order documented here.
