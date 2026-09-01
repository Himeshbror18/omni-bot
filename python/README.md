# 🐍 Omni-Bot Python Control Center

The Python companion for the **Omni-Bot ESP32 firmware**.

It is designed to sit alongside the robot firmware rather than replace it: the ESP32 remains responsible for real-time motor control, while Python provides a desktop control/diagnostic layer over Wi-Fi.

## Planned capabilities

- 🎮 Keyboard and gamepad control
- 🕹️ Analog joystick support
- ⚡ Speed limiting
- 🎯 Per-wheel calibration
- 🔧 Individual motor diagnostics
- 📊 Telemetry dashboard
- 🧪 Offline mecanum simulation
- 🛑 Communication watchdog / emergency stop
- 📝 Command logging

## Architecture

```text
Python Control Center
        │
        │ HTTP / future WebSocket
        ▼
   WeMos D1 R32
        │
        ▼
  TB6612FNG drivers
        │
        ▼
    4 × motors
```

The Python application should never assume that a network command arrived successfully. Commands are sent with bounded speed values, and the controller is designed to fail safe by stopping the robot when communication is lost.

## Setup

Create a virtual environment and install the dependencies:

```bash
python -m venv .venv
# Windows
.venv\Scripts\activate
# Linux/macOS
source .venv/bin/activate

pip install -r requirements.txt
```

Run:

```bash
python -m omnibot
```

> The current repository firmware exposes HTTP endpoints. The Python package is intentionally structured so the transport layer can later move to WebSocket without rewriting the control logic.
