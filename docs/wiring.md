# Wiring — WeMos D1 R32 + 2× TB6612FNG

## Logic
- D1 R32 3V3 → both TB6612 VCC
- D1 R32 GND → both driver GND
- GPIO32 → Driver A STBY
- GPIO33 → Driver B STBY

## Driver A
- GPIO13 → PWMA → FL
- GPIO18 → AIN1
- GPIO19 → AIN2
- GPIO14 → PWMB → FR
- GPIO21 → BIN1
- GPIO22 → BIN2
- A01/A02 → FL motor
- B01/B02 → FR motor

## Driver B
- GPIO16 → PWMA → RL
- GPIO23 → AIN1
- GPIO25 → AIN2
- GPIO17 → PWMB → RR
- GPIO26 → BIN1
- GPIO27 → BIN2
- A01/A02 → RL motor
- B01/B02 → RR motor

Motor battery positive → both VM inputs. Motor battery negative → common GND. Never power motors from the ESP32 3.3 V rail.

## First test
1. Keep wheels off the ground.
2. Power up.
3. Connect to Omni-Bot Wi-Fi.
4. Test each motor at low speed.
5. Confirm physical direction.
6. Adjust inversion flags if necessary.
7. Test combined movement.

Verify your exact breakout-board labels and electrical ratings before wiring.
