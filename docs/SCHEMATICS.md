# Omni-Bot Schematics

This document is the wiring and electrical reference for the **Omni-Bot built specifically around the WeMos D1 R32 (ESP32)**.

> **Controller:** WeMos D1 R32 only. Do not substitute an Arduino Uno, Mega, NodeMCU ESP8266, or another ESP32 board without remapping and re-validating the wiring.

## 1. System overview

```text
                         USB / 5V regulated supply
                                  │
                                  ▼
                         ┌───────────────────┐
                         │  WeMos D1 R32     │
                         │     ESP32         │
                         │                   │
                         │ GPIO control      │
                         └───────┬───────────┘
                                 │ 3.3V logic
                    ┌────────────┴────────────┐
                    │                         │
                    ▼                         ▼
            ┌───────────────┐         ┌───────────────┐
            │ TB6612FNG A   │         │ TB6612FNG B   │
            │ FL + FR       │         │ RL + RR       │
            └──────┬────────┘         └──────┬────────┘
                   │                         │
              ┌────┴────┐               ┌────┴────┐
              ▼         ▼               ▼         ▼
             FL        FR              RL        RR
            motor     motor           motor     motor
                   ▲                         ▲
                   │                         │
                   └───────────┬─────────────┘
                               │ VM
                        Motor battery

All grounds are common:
WeMos GND ─ TB6612 A GND ─ TB6612 B GND ─ Battery −
```

## 2. Exact pin mapping

| Motor | Driver channel | PWM | IN1 | IN2 | Firmware inversion |
|---|---|---:|---:|---:|---|
| Front Left (FL) | Driver A A-channel | GPIO13 | GPIO18 | GPIO19 | No |
| Front Right (FR) | Driver A B-channel | GPIO14 | GPIO21 | GPIO22 | Yes |
| Rear Left (RL) | Driver B A-channel | GPIO16 | GPIO23 | GPIO25 | No |
| Rear Right (RR) | Driver B B-channel | GPIO17 | GPIO26 | GPIO27 | Yes |

Standby:

- Driver A `STBY` → **GPIO32**
- Driver B `STBY` → **GPIO33**

Logic/power:

- WeMos D1 R32 `3V3` → both TB6612FNG `VCC` logic inputs
- WeMos D1 R32 `GND` → both driver `GND`
- Motor battery `+` → both driver `VM`
- Motor battery `−` → common `GND`
- Motor outputs `A01/A02` or `B01/B02` → the corresponding motor

## 3. Controller → driver signal table

```text
WeMos D1 R32              TB6612FNG A
─────────────             ────────────
GPIO13  ─────────────────► PWMA  ─► FL PWM
GPIO18  ─────────────────► AIN1
GPIO19  ─────────────────► AIN2
GPIO14  ─────────────────► PWMB  ─► FR PWM
GPIO21  ─────────────────► BIN1
GPIO22  ─────────────────► BIN2
GPIO32  ─────────────────► STBY
3V3     ─────────────────► VCC
GND     ─────────────────► GND

WeMos D1 R32              TB6612FNG B
─────────────             ────────────
GPIO16  ─────────────────► PWMA  ─► RL PWM
GPIO23  ─────────────────► AIN1
GPIO25  ─────────────────► AIN2
GPIO17  ─────────────────► PWMB  ─► RR PWM
GPIO26  ─────────────────► BIN1
GPIO27  ─────────────────► BIN2
GPIO33  ─────────────────► STBY
3V3     ─────────────────► VCC
GND     ─────────────────► GND
```

## 4. Wheel arrangement

Viewed from above, with the front of the robot at the top:

```text
                 FRONT
        ┌─────────────────────┐
        │  FL             FR  │
        │   ╲             ╱   │
        │                     │
        │   ╱             ╲   │
        │  RL             RR  │
        └─────────────────────┘
                  REAR
```

For an X-pattern mecanum arrangement, the roller directions form an X when viewed from above. The physical wheel handedness still must be checked on the actual wheel set.

## 5. Power topology

Keep **logic power and motor power conceptually separate**:

- The WeMos D1 R32 is the controller and Wi-Fi computer.
- The TB6612FNG `VM` rail supplies motor power from the motor battery.
- The TB6612FNG `VCC` rail supplies its logic from the D1 R32 `3V3` rail as documented here.
- Grounds must be connected together so the control signals have a common reference.
- **Never connect the motor battery directly to an ESP32 GPIO or the D1 R32 3.3 V pin.**
- Do not assume a breakout-board jumper or regulator is harmless. Verify what it connects before applying power.

## 6. Commissioning sequence

1. Lift the robot so every wheel is off the floor.
2. Check the wiring against this document pin-for-pin.
3. Power the WeMos D1 R32 and the motor drivers.
4. Connect to the `Omni-Bot` Wi-Fi access point.
5. Test FL, FR, RL and RR individually at low speed.
6. Confirm that the physical wheel direction matches the expected wheel direction.
7. Correct motor polarity or firmware inversion only after confirming the wiring.
8. Test forward/reverse.
9. Test left/right strafe.
10. Test clockwise/counter-clockwise rotation.
11. Only then place the robot on the floor for combined-motion tests.
