# 🤖 Omni-Bot
### WeMos D1 R32 • ESP32 • 4-Wheel Omni/Mecanum Drive • Wi-Fi Web Control

<p align="center"><strong>A compact, hackable ESP32 robot with browser-based control.</strong><br>Connect to the robot over Wi-Fi and drive it from a phone or computer.</p>

![Omni-Bot wiring overview](docs/wiring-diagram.svg)

## ✨ Overview

Omni-Bot is a four-wheel omni/mecanum robot built around one specific controller: the **WeMos D1 R32 (ESP32)**. Two dual-channel **TB6612FNG** motor drivers provide four independent motor channels.

The D1 R32 handles Wi-Fi, the web controller, movement commands and the final wheel-speed mix. The Python package provides a higher-level desktop control/diagnostic interface over the same HTTP API.

> **Hardware target:** This repository is documented and configured for the **WeMos D1 R32**. Do not substitute a different board without creating and validating a new pin map. The robot has enough variables already.

## 🚨 Power and logic warning

The WeMos D1 R32 uses an ESP32 with **3.3 V GPIO logic**. Do not apply motor-battery voltage or an unverified 5 V signal to an ESP32 GPIO.

Before powering the robot, verify the exact TB6612FNG breakout-board schematic, `VCC` requirements, motor-supply limits, jumpers/solder bridges and input thresholds. The driver board's `VM` motor supply is separate from its logic `VCC` supply.

**Never power the four motors from the D1 R32 3.3 V rail.** Motor current belongs on the motor-supply path through the TB6612FNG drivers. The ESP32 is a controller, not a four-motor power bank with Wi-Fi.

## 🧩 Hardware

| Part | Role |
|---|---|
| **WeMos D1 R32** | Main controller + Wi-Fi |
| **2× TB6612FNG** | Four H-bridge motor channels |
| **4× DC gear motors** | Omni/mecanum drive |
| **Motor battery** | Motor power through `VM` |
| **Regulated controller supply** | Power for the D1 R32 |

## 🛞 Wheel layout

Viewed from above, front at the top:

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

`FL` = Front Left · `FR` = Front Right · `RL` = Rear Left · `RR` = Rear Right

For the usual X-pattern mecanum layout, the roller directions form an X when viewed from above. Confirm the handedness of the actual wheel set before final assembly. Four wheels spinning confidently in the wrong arrangement is still wrong.

## 🎮 Movement model

The firmware uses:

```text
X = forward (+) / reverse (-)
Y = right (+) / left (-)
R = clockwise (+) / counter-clockwise (-)

FL = X - Y - R
FR = X + Y + R
RL = X + Y - R
RR = X - Y + R
```

The web controller supports forward, reverse, left/right strafe, four diagonals, clockwise/counter-clockwise rotation and STOP.

## 🔌 Wiring and schematics

The detailed wiring reference is kept in one dedicated document:

- **[`docs/SCHEMATICS.md`](docs/SCHEMATICS.md)** — definitive controller, driver, power, signal and wheel-layout reference.
- **[`docs/wiring.md`](docs/wiring.md)** — concise pin-by-pin wiring checklist.
- **[`docs/motor-positioning.md`](docs/motor-positioning.md)** — wheel orientation and movement convention.

### Pin map

| Motor | PWM | IN1 | IN2 | Driver |
|---|---:|---:|---:|---|
| FL | GPIO13 | GPIO18 | GPIO19 | TB6612FNG A, channel A |
| FR | GPIO14 | GPIO21 | GPIO22 | TB6612FNG A, channel B |
| RL | GPIO16 | GPIO23 | GPIO25 | TB6612FNG B, channel A |
| RR | GPIO17 | GPIO26 | GPIO27 | TB6612FNG B, channel B |

Standby: **Driver A `STBY` → GPIO32**, **Driver B `STBY` → GPIO33**.

Logic: **D1 R32 `3V3` → both driver `VCC`** and **all grounds common**.  
Motor power: battery positive → driver `VM`; battery negative → common ground.

![Omni-Bot schematic](docs/schematic.svg)

![Omni-Bot detailed wiring](docs/wiring-diagram.svg)

## 🌐 Web controller

The D1 R32 starts its own Wi-Fi access point:

```text
SSID:      Omni-Bot
Password:  omnibot123
Address:   http://192.168.4.1
```

The browser controller provides:

- Direction and diagonal driving
- Rotation
- Speed control
- Individual motor testing
- Explicit STOP
- Stop behavior on browser focus/visibility loss

Hold a direction button to move. Releasing it sends a stop command. Because asking the robot to keep driving after you let go seemed like a poor default.

## 🧠 Controller architecture

```text
Phone / Browser
      │
      │ HTTP
      ▼
WeMos D1 R32 (ESP32)
      │
      │ GPIO + PWM
      ▼
2 × TB6612FNG
      │
      ├── FL
      ├── FR
      ├── RL
      └── RR
```

The ESP32 firmware is the authoritative low-level controller. The Python `Controller` converts keyboard input to the same movement commands used by the web interface.

See **[`python/README.md`](python/README.md)** for the controller/protocol reference.

## 🐍 Python Control Center

The Python companion communicates with the D1 R32 over HTTP:

```text
GET /move?c=<command>&v=<0..100>
GET /motor?i=<0..3>&v=<-100..100>
```

Keyboard mapping:

```text
W → forward          S → reverse
A → strafe left      D → strafe right
Q → rotate CCW       E → rotate CW
U → forward-left     I → forward-right
J → backward-left    K → backward-right
```

The four motor indexes are `0=FL`, `1=FR`, `2=RL`, `3=RR`.

### Quick start

```bash
cd python
python -m venv .venv

# Windows
.venv\\Scripts\\activate

# Linux/macOS
source .venv/bin/activate

pip install -r requirements.txt
python -m omnibot
```

## 💻 Firmware

```text
firmware/
├── platformio.ini
└── src/
    └── main.cpp
```

PlatformIO is configured for the **WeMos D1 R32**. Build/upload with PlatformIO, power the robot safely, connect to the `Omni-Bot` access point and open `http://192.168.4.1`.

## 🧪 First power-up / commissioning

1. Lift all wheels off the ground.
2. Check the wiring against [`docs/SCHEMATICS.md`](docs/SCHEMATICS.md).
3. Power the D1 R32 and drivers with the correct supplies.
4. Connect to the `Omni-Bot` Wi-Fi network.
5. Test FL, FR, RL and RR individually at low speed.
6. Verify each wheel's physical direction.
7. Correct motor polarity or firmware inversion only after checking the wiring.
8. Test forward/reverse, then strafe and rotation.
9. Test combined motion with the wheels still lifted.
10. Only then run the robot on the floor.

The floor is a much less forgiving debugging environment.

## 🧯 Common failure modes

| Symptom | Likely cause |
|---|---|
| D1 R32 resets when motors start | Supply sag, noise, grounding or regulator problem |
| One wheel runs backwards | Motor polarity or inversion setting |
| Strafing is wrong | Wheel orientation, wiring, inversion or mixer issue |
| Driver gets very hot | Motor current/load exceeds practical driver limits |
| Wi-Fi works but motors do not | Driver power, `STBY`, wiring or logic-supply issue |
| Controller is damaged after driver connection | Possible overvoltage/back-feed; stop and inspect immediately |

For electrical problems, measure voltage and continuity instead of guessing from symptoms. Multimeters remain annoyingly useful.

## 📁 Repository

```text
omni-bot/
├── README.md
├── docs/
│   ├── SCHEMATICS.md
│   ├── motor-positioning.md
│   ├── wiring.md
│   ├── schematic.svg
│   └── wiring-diagram.svg
├── firmware/
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
└── python/
    ├── README.md
    ├── pyproject.toml
    ├── requirements.txt
    └── omnibot/
        ├── __init__.py
        ├── __main__.py
        ├── controller.py
        ├── mecanum.py
        └── protocol.py
```

## ☕ Support

If this project helps you build something cool, you can support my work on **Ko-fi**.

<p align="center"><a href="https://ko-fi.com/himanshu18"><img src="https://img.shields.io/badge/Support%20me%20on-Ko--fi-ff5e5b?style=for-the-badge&logo=ko-fi&logoColor=white" alt="Support on Ko-fi"></a></p>

## 📜 License

Released under the **MIT License**.

<p align="center"><sub>Made with ❤️ by Himeshbror18</sub></p>
