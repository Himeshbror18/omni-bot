# 🐍 Omni-Bot Python Control Center

The Python companion for the **Omni-Bot firmware running on the WeMos D1 R32 (ESP32)**.

The architecture is deliberately split into two layers:

```text
Desktop Python controller
        │
        │ HTTP commands over Wi-Fi
        ▼
   WeMos D1 R32
       ESP32
        │
        │ GPIO + PWM
        ▼
  2 × TB6612FNG
        │
        ▼
    4 × DC motors
```

The D1 R32 is the robot's real-time controller. Python is a higher-level command and diagnostic layer; it does **not** directly generate the four-wheel motor mix in the current firmware API.

## Controller mapping

The keyboard mapping in `omnibot/controller.py` matches the firmware's `/move` commands:

| Key | Command | Action |
|---|---|---|
| `W` | `f` | Forward |
| `S` | `b` | Reverse |
| `A` | `l` | Strafe left |
| `D` | `r` | Strafe right |
| `Q` | `ccw` | Rotate counter-clockwise |
| `E` | `cw` | Rotate clockwise |
| `U` | `fl` | Forward-left diagonal |
| `I` | `fr` | Forward-right diagonal |
| `J` | `bl` | Backward-left diagonal |
| `K` | `br` | Backward-right diagonal |

Any unmapped key is treated as a stop command by the current high-level controller.

## HTTP protocol

The current WeMos D1 R32 firmware exposes:

```text
GET /move?c=<command>&v=<0..100>
GET /motor?i=<0..3>&v=<-100..100>
```

Default robot address:

```text
http://192.168.4.1
```

The four motor indexes are:

```text
0 = FL   1 = FR   2 = RL   3 = RR
```

The `protocol.py` transport layer clamps speed values, uses a bounded request timeout, and provides an explicit `stop()` operation.

## Mecanum model

`omnibot/mecanum.py` documents the same wheel order and mixer used by the firmware:

```text
FL = X - Y - R
FR = X + Y + R
RL = X + Y - R
RR = X - Y + R
```

Where:

- `X` = forward (+) / reverse (-)
- `Y` = right (+) / left (-)
- `R` = clockwise rotation (+) / counter-clockwise (-)

This module is useful for simulation, higher-level planning, or future controllers. The current HTTP API sends motion commands to the D1 R32, which performs the final mixing and output.

## Safety behavior

The browser controller stops on focus loss or when the document becomes hidden. The Python client also exposes an explicit stop operation and uses short HTTP timeouts. A network timeout should be treated as a reason to stop, not as permission to continue driving indefinitely.

## Setup

```bash
python -m venv .venv

# Windows
.venv\Scripts\activate

# Linux/macOS
source .venv/bin/activate

pip install -r requirements.txt
python -m omnibot
```

The Python package is kept transport-oriented so the HTTP layer can be extended later without coupling the desktop controls to the motor-driver GPIO details.
