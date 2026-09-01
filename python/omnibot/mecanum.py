"""Mecanum/omni wheel mixing.

Inputs are normalized to -1..1:
    x: forward (+) / reverse (-)
    y: right (+) / left (-)
    rotation: clockwise (+) / counter-clockwise (-)

The wheel order is FL, FR, RL, RR and matches the firmware mixer.
"""

from dataclasses import dataclass


@dataclass(frozen=True)
class WheelSpeeds:
    fl: float
    fr: float
    rl: float
    rr: float

    def normalized(self) -> "WheelSpeeds":
        peak = max(1.0, abs(self.fl), abs(self.fr), abs(self.rl), abs(self.rr))
        return WheelSpeeds(
            self.fl / peak, self.fr / peak, self.rl / peak, self.rr / peak
        )


def mix(x: float, y: float, rotation: float) -> WheelSpeeds:
    x = max(-1.0, min(1.0, x))
    y = max(-1.0, min(1.0, y))
    rotation = max(-1.0, min(1.0, rotation))
    return WheelSpeeds(
        x - y - rotation,
        x + y + rotation,
        x + y - rotation,
        x - y + rotation,
    ).normalized()
