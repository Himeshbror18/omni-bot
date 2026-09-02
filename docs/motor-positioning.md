# Motor Positioning — WeMos D1 R32 Omni-Bot

This document defines the physical wheel names and coordinate convention used by the current **WeMos D1 R32** firmware and Python controller.

## Wheel layout

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

- **FL** — Front Left
- **FR** — Front Right
- **RL** — Rear Left
- **RR** — Rear Right

For an X-pattern mecanum arrangement, the roller directions form an X when viewed from above. The actual wheel handedness must be checked against the physical wheel set.

## Coordinate convention

```text
+X = forward
-X = reverse
+Y = right strafe
-Y = left strafe
+R = clockwise rotation
-R = counter-clockwise rotation
```

## Wheel mixer

The firmware uses:

```text
FL = X - Y - R
FR = X + Y + R
RL = X + Y - R
RR = X - Y + R
```

The Python `mecanum.py` module uses the same equations and wheel order.

## Direction verification

Motor electrical direction is not enough to prove that the complete robot is wired correctly. With the wheels lifted:

1. Test each wheel individually at low speed.
2. Record which physical direction each wheel turns.
3. Confirm forward command produces the intended forward wheel behavior.
4. Confirm left/right strafing works only after the wheel directions and roller orientations are correct.
5. Use the existing firmware inversion flags where needed rather than changing the documented pin map.
