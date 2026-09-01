# 🚗 Omni-Bot
### WeMos D1 R32 • ESP32 • 4-Wheel Omni/Mecanum Drive • Wi-Fi Web Control

<p align="center"><strong>A compact, hackable ESP32 robot with browser-based control.</strong><br>No dedicated mobile app — connect to the robot and drive it from your phone.</p>

![Omni-Bot wiring overview](docs/wiring-diagram.svg)

## ✨ What is Omni-Bot?
Omni-Bot is a four-wheel omni-directional robot built around the **WeMos D1 R32 (ESP32)**. Two dual-channel **TB6612FNG** motor drivers independently control the four motors, while the ESP32 hosts a lightweight Wi-Fi web interface for driving, speed adjustment, rotation and individual motor testing.

## 🚨 READ THIS BEFORE CONNECTING POWER

### ⚠️ ESP32 = 3.3 V logic. Do **not** feed 5 V into its GPIO pins.

The WeMos D1 R32 uses an ESP32, whose GPIOs are **not 5 V tolerant**. A motor driver being powered from 5 V does **not automatically** mean its inputs will damage the ESP32; the driver's actual input thresholds and board design matter. The real danger is putting an out-of-spec voltage onto an ESP32 GPIO, including accidentally back-feeding 5 V through a driver board.

### 🔥 Beware the jumper / regulator trap

Some motor-driver breakout boards have jumpers, solder bridges, or onboard regulators that connect the motor supply and logic supply in ways that are not obvious.

If you externally supply a driver with 5 V and leave the wrong jumper installed, that 5 V may appear on a logic rail or signal path.

**Before powering anything:**

1. Identify the exact driver-board model.
2. Read its schematic/datasheet.
3. Check what **VCC** expects.
4. Check whether its inputs accept a 3.3 V HIGH.
5. Check every jumper/solder bridge and what it connects.
6. Verify that no 5 V rail can reach an ESP32 GPIO.
7. Make all required grounds common.
8. Only then connect the motor battery.

> 🔴 **Do not assume a jumper is "just a jumper."** It may be a tiny piece of wire with a very large capacity for ruining your afternoon.
>
> If the driver really requires 5 V logic, use a proper **3.3 V↔5 V level shifter** instead of hoping the ESP32 develops 5 V tolerance through positive thinking.

## 🧩 Core hardware
| Part | Role |
|---|---|
| **WeMos D1 R32** | Main controller + Wi-Fi |
| **2× TB6612FNG** | Four independent H-bridge channels |
| **4× DC gear motors** | Omni/mecanum drive |
| **Motor battery** | Motor power |
| **Regulated ESP32 supply** | Logic power |

> ⚠️ **Hardware note:** This repository assumes TB6612FNG breakout boards. Verify the exact board pin labels, motor voltage/current and battery voltage before connecting power.

## 🛞 Wheel layout
```text
                 FRONT
        ┌─────────────────────┐
        │   FL           FR   │
        │    \\           //   │
        │                     │
        │    //           \\   │
        │   RL           RR   │
        └─────────────────────┘
                  REAR
```
FL = Front Left · FR = Front Right · RL = Rear Left · RR = Rear Right

For the common **X-pattern mecanum configuration**, the roller orientation forms an X when viewed from above.

## 🎮 Movement model
X = forward/reverse · Y = lateral strafe · R = rotation

```text
FL = X - Y - R
FR = X + Y + R
RL = X + Y - R
RR = X - Y + R
```

Supported controls: **↑ ↓ ← → · ↖ ↗ ↙ ↘ · ↺ ↻ · STOP**

Individual FL / FR / RL / RR controls are included for commissioning and troubleshooting.

## 🔌 Wiring & schematics
### Block schematic
![Omni-Bot schematic](docs/schematic.svg)

### Detailed wiring
![Omni-Bot detailed wiring](docs/wiring-diagram.svg)

### Pin map
| Motor | PWM | IN1 | IN2 | Driver |
|---|---:|---:|---:|---|
| FL | GPIO13 | GPIO18 | GPIO19 | A |
| FR | GPIO14 | GPIO21 | GPIO22 | A |
| RL | GPIO16 | GPIO23 | GPIO25 | B |
| RR | GPIO17 | GPIO26 | GPIO27 | B |

Driver A STBY → GPIO32 · Driver B STBY → GPIO33

TB6612 VCC → 3.3V logic · VM → motor battery · all grounds common.

### 🔴 Power warning

> Never power the motors from the ESP32 3.3 V rail. The ESP32 is a controller, not a tiny four-motor power supply pretending to be one.

## 🌐 Web controller
The ESP32 creates its own Wi-Fi access point:

```text
SSID:      Omni-Bot
Password:  omnibot123
Address:   http://192.168.4.1
```

Features include a direction pad, diagonal movement, rotation, speed control, individual motor testing and stop control.

## 🧰 Features & upgrade roadmap

The current firmware provides **Wi-Fi AP mode, a mobile touch controller, directional movement, diagonals, rotation, speed control, individual motor testing, and safety stops when the browser loses focus**.

Possible upgrades:

- 🎚️ Joystick-style analog driving
- 🎯 Per-wheel calibration
- 🐢 Acceleration/deceleration ramping
- 🔋 Battery-voltage monitoring and low-battery warnings
- 📊 Live telemetry
- 🎮 Bluetooth/gamepad control
- 📡 Station-mode Wi-Fi
- 🔐 Configurable Wi-Fi credentials
- 🧭 IMU-assisted heading control
- 🛑 Hardware emergency-stop input
- 🤖 Autonomous movement

Some upgrades require additional hardware. Sadly, software cannot manufacture an IMU out of optimism.

## 🐍 Python Control Center

The **Python companion lives inside this repository** and is designed to work alongside the ESP32 firmware.

The ESP32 remains the low-level motor controller; Python is the higher-level desktop control and diagnostics layer.

### Current Python foundation

- 🎮 Keyboard control
- 📡 Wi-Fi HTTP communication with the ESP32
- 🧮 Mecanum wheel-mixing module matching the firmware
- 🔧 Individual motor commands
- 🛑 Explicit stop command
- 📦 Installable Python package structure
- 🧪 Ready for GUI, gamepad, telemetry and simulation

### Quick start

```bash
cd python
python -m venv .venv

# Windows
.venv\Scripts\activate

# Linux/macOS
source .venv/bin/activate

pip install -r requirements.txt
python -m omnibot
```

> If the computer disappears from Wi-Fi, the controller is designed around explicit stop handling rather than assuming silence means “keep driving.” Because apparently robots need boundaries too.

See [`python/README.md`](python/README.md) for the Python architecture.
## 💻 Firmware
```text
firmware/
├── platformio.ini
└── src/
    └── main.cpp
```

Build with PlatformIO, upload over USB, connect to the Omni-Bot network and open the controller address.

### First power-up
1. Lift all wheels off the ground.
2. Power the controller and drivers.
3. Connect to the web interface.
4. Test every motor individually at low speed.
5. Verify physical direction and adjust inversion flags if necessary.
6. Test combined movement before placing the robot on the floor.

## 🧯 Common failure modes

| Symptom | Likely cause |
|---|---|
| ESP32 resets when motors start | Supply sag, noise, or poor power arrangement |
| One motor runs backwards | Motor polarity or inversion setting |
| Robot refuses to strafe correctly | Wheel orientation, inversion, or mixer issue |
| Driver gets very hot | Motor current/load exceeds the driver's practical limit |
| ESP32 gets hot | 🚨 Stop powering it and investigate immediately |
| Wi-Fi works but motors don't | Driver power, STBY, or wiring problem |
| ESP32 dies after connecting a driver | 🚨 Possible overvoltage/back-feed — inspect the wiring |

Robotics debugging is mostly measurement, documentation, and occasionally staring at a wire while asking what it did to deserve this.

## 📁 Repository
```text
omni-bot/
├── README.md
├── python/
│   ├── README.md
│   ├── pyproject.toml
│   ├── requirements.txt
│   └── omnibot/
│       ├── __init__.py
│       ├── __main__.py
│       ├── controller.py
│       ├── mecanum.py
│       └── protocol.py
├── docs/
│   ├── motor-positioning.md
│   ├── wiring.md
│   ├── schematic.svg
│   └── wiring-diagram.svg
└── firmware/
    ├── platformio.ini
    └── src/
        └── main.cpp
```

## ☕ Support
If this project helps you build something cool, you can support my work on **Ko-fi**.

<p align="center"><a href="https://ko-fi.com/himanshu18"><img src="https://img.shields.io/badge/Support%20me%20on-Ko--fi-ff5e5b?style=for-the-badge&logo=ko-fi&logoColor=white" alt="Support on Ko-fi"></a></p>

This is the same Ko-fi linked from my **Cursed Archive** project.

## 📜 License
Released under the **MIT License**.

In short: you can use, modify, redistribute and build your own projects from this code, while keeping the license/copyright notice. The license does not magically protect your hardware from 5 V mistakes.

<p align="center"><sub>Made with ❤️ by Himeshbror18</sub></p>