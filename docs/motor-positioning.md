# Motor Positioning

Viewed from above, front at the top:

```text
        FRONT
    +----------------+
    | FL          FR |
    |  \\          // |
    |                |
    |  //          \\ |
    | RL          RR |
    +----------------+
         REAR
```

FL = front-left, FR = front-right, RL = rear-left, RR = rear-right.

For the common X-pattern mecanum arrangement, the roller orientation forms an X when viewed from above. Confirm your wheel set's handedness before mounting.

Movement convention:
- +X = forward
- -X = reverse
- +Y = strafe right
- -Y = strafe left
- +R = clockwise rotation

Motor mix:
- FL = X - Y - R
- FR = X + Y + R
- RL = X + Y - R
- RR = X - Y + R

The firmware has per-motor inversion flags to compensate for electrical direction. Verify all four wheels at low speed before driving.
