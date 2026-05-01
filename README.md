# 🤖 Line Follower Robot — Arduino UNO

> An autonomous differential-drive robot that detects and precisely follows a black line using an IR sensor array and a PD control algorithm — built and competed at the university level.

---

## 📌 Table of Contents

- [Overview](#overview)
- [How It Works — System Architecture](#how-it-works--system-architecture)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [Code Structure](#code-structure)
- [Algorithm Explained](#algorithm-explained)
- [Getting Started](#getting-started)
- [Tuning the PD Controller](#tuning-the-pd-controller)
- [Author](#author)
- [License](#license)

---

## 📖 Overview

This project is a **Line Follower Robot** developed for a university robotics competition. The robot autonomously navigates a track by detecting a black line on a surface using an array of **14 IR sensors** (read via a multiplexer), computing the line's weighted position, and applying a **PD (Proportional-Derivative) control algorithm** to continuously correct the motor speeds.

The entire system is built on an **Arduino UNO** microcontroller. The code is modular — split across clearly separated files for sensor reading, motor driving, and line-following logic — making it easy to understand, modify, and extend.

---

## ⚙️ How It Works — System Architecture

The robot runs on a closed-loop feedback cycle:

```
┌─────────────────────────────────────────────────────────────┐
│                     MAIN LOOP (loop())                      │
│                                                             │
│  1. READ SENSORS  ──►  2. COMPUTE POSITION  ──►  3. PD     │
│     (14 IR sensors         (weighted average         control│
│      via Multiplexer)       of active sensors)       logic  │
│                                                       │     │
│                    6. REPEAT  ◄──  5. DRIVE  ◄────────┘     │
│                                    motors                    │
└─────────────────────────────────────────────────────────────┘
```

**Step-by-step:**

1. **Sensor Reading** — 14 IR sensors are read sequentially through a **4-bit multiplexer** (controlled by pins S0–S3). Each sensor returns an analog value.
2. **Normalization** — Raw sensor values are mapped to a calibrated range using each sensor's recorded `minimum[]` and `maximum[]` values.
3. **Weighted Position** — Active sensor readings are combined into a single **position value** (0–13,000) representing where the line currently sits under the robot.
4. **Error Calculation** — The error is computed as the deviation from the center target (position `6500`).
5. **PD Correction** — A PD controller uses the current error and the change in error from the last cycle to calculate a speed adjustment.
6. **Motor Drive** — Left and right motor speeds are updated accordingly and clamped within `minSpeed` and `maxSpeed` bounds.

---

## 🔧 Hardware Requirements

| Component | Details |
|---|---|
| Arduino UNO | Main microcontroller |
| IR Sensor Array (×14) | Analog reflectance sensors for line detection |
| 4-bit Analog Multiplexer (e.g., CD74HC4067) | Reads 14 sensors through a single analog pin |
| DC Gear Motors (×2) | Left and right drive wheels |
| Motor Driver (e.g., L298N) | Controls motor direction and speed via PWM |
| Chassis + Wheels + Caster | Robot body |
| Li-Po Battery | Power supply for motors and Arduino |
| Jumper Wires | Connections |

---

## 🔌 Pin Configuration

### Motor Driver Pins

| Label | Arduino Pin | Purpose |
|---|---|---|
| `lmf` | D8 | Left motor — forward direction |
| `lmb` | D7 | Left motor — backward direction |
| `lmPWM` | D6 | Left motor — speed control (PWM) |
| `rmf` | D9 | Right motor — forward direction |
| `rmb` | D10 | Right motor — backward direction |
| `rmPWM` | D11 | Right motor — speed control (PWM) |

### Multiplexer Address Pins (Sensor Array)

| Label | Purpose |
|---|---|
| `S0` | MUX address bit 0 |
| `S1` | MUX address bit 1 |
| `S2` | MUX address bit 2 |
| `S3` | MUX address bit 3 |

> The analog output of the MUX is read on **Arduino pin A0**.

---

## 📂 Code Structure

The project is split into **5 modular `.ino` files**. The Arduino IDE automatically compiles all `.ino` files in the same folder as a single unified sketch.

```
Line-Follower-Robot/
│
├── Line_following_Codes.ino   ← Main entry point: setup(), loop(), pin definitions, global variables
├── Sensor_Codes.ino           ← reading() — basic 6-sensor reader with threshold & weighted average
├── 4readSensor.ino            ← readLine() — advanced 14-sensor MUX reader with calibration & normalization
├── 3lineFollow.ino            ← lineFollow() — PD controller, computes corrected motor speeds
└── Motor_Driving_Codes.ino   ← mtr() — controls motor direction and PWM speed output
```

---

## 🧠 Algorithm Explained

### File: `Line_following_Codes.ino` — Main Sketch

This is the **entry point** of the entire program. It contains:

- Pin definitions for both motors
- Global variables: sensor arrays, position values, PD state variables
- `setup()` — Initializes Serial at 9600 baud, sets sensor pins (A0–A5) as INPUT, motor pins as OUTPUT
- `loop()` — Calls `semi_pid()` in an infinite loop to continuously follow the line

---

### File: `Sensor_Codes.ino` — Basic 6-Sensor Reader

```cpp
void reading()
```

Reads **6 analog IR sensors** directly from Arduino pins A0–A5:

- Applies a **detection threshold of 350** — analog values above 350 are treated as `1` (line detected), values below as `0` (no line)
- Computes a **weighted average** position using predefined sensor positions `{1, 2, 3, 4, 5, 6}`
- Stores the result in `avg`, which represents the line's position across the sensor array

> This is an early/simplified version of sensor reading, forming the conceptual foundation for the more advanced implementation below.

---

### File: `4readSensor.ino` — Advanced 14-Sensor MUX Reader

```cpp
uint32_t readLine()
```

Reads **14 IR sensors** through a 4-bit analog multiplexer:

- Calls `Mux_Addr(i)` to select each sensor channel (0–13) sequentially
- Reads the analog value from the MUX output
- **Normalizes** each reading against per-sensor calibration data using `mapC()`, mapping raw values to a 0–1023 scale
- Computes a **weighted position average** — sensors further to the right carry higher weight, giving a position between `0` and `13,000`
- Center of the track corresponds to position **`6,500`**
- **Line-lost recovery**: if no sensor detects the line (`on_line == 0`), returns the extreme position (0 or 13,000) based on which side the line was last seen, helping the robot recover without stopping

```cpp
void Mux_Addr(int ch_Addr)
```
Sets the 4 address lines (S0–S3) of the MUX to select a specific sensor channel.

```cpp
int mapC(int x, int in_min, int in_max, int out_min, int out_max)
```
Custom linear mapping function used to normalize raw sensor values against stored calibration min/max values.

---

### File: `3lineFollow.ino` — PD Controller

```cpp
void lineFollow(unsigned int position, int baseSpeedC, int maxSpeed, int minSpeed, float Kp, float Kd, float Ki)
```

This is the **brain of the robot**. It implements a **PD (Proportional-Derivative) controller**:

| Term | Formula | Role |
|---|---|---|
| **Error** | `6500 − position` | How far off-center the line currently is |
| **Proportional (P)** | `Kp × error` | Corrects based on the magnitude of the current error |
| **Derivative (D)** | `Kd × (error − lastError)` | Corrects based on how quickly the error is changing — dampens oscillation |

**Motor speed adjustment:**

```
adjustPosition  = (Kp × error) + (Kd × (error − lastError))
Left  Motor Speed = baseSpeed + adjustPosition
Right Motor Speed = baseSpeed − adjustPosition
```

Both motor speeds are then **clamped** between `minSpeed` and `maxSpeed` to prevent hardware damage and maintain stable tracking.

> Although `Ki` (Integral gain) is accepted as a parameter, the current implementation uses only **P and D terms**, making this a PD controller. The `Ki` parameter is left in place for easy future extension to a full PID controller.

---

### File: `Motor_Driving_Codes.ino` — Motor Driver

```cpp
void mtr(int left, int right)
```

Handles the low-level motor control:

- **Positive value** → motor rotates forward (sets forward pin HIGH, backward pin LOW)
- **Negative value** → motor rotates backward (sets backward pin HIGH, forward pin LOW), and the sign is flipped for PWM
- Calls `analogWrite()` on PWM pins to set the actual speed (0–255)

This abstraction allows the PD controller to simply pass signed speed values without worrying about direction logic.

---

## 🚀 Getting Started

### Step 1 — Clone the Repository
```bash
git clone https://github.com/your-username/line-follower-robot.git
cd line-follower-robot
```

### Step 2 — Open in Arduino IDE
- Launch **Arduino IDE** (v1.8.x or later — [Download](https://www.arduino.cc/en/software))
- Go to `File → Open` and select the main `.ino` file
- Arduino IDE will automatically load all other `.ino` files in the same folder as separate tabs

### Step 3 — Connect Your Arduino UNO
- Connect via USB to your PC

### Step 4 — Select Board & Port
```
Tools → Board → Arduino UNO
Tools → Port → COM# (your port)
```

### Step 5 — Upload
- Click the **Upload** button (→)
- Wait for **"Done uploading"** in the status bar

### Step 6 — Run
- Disconnect from the PC, place the robot on the line track, and power it on
- The robot will immediately begin following the line

---

## 🎛️ Tuning the PD Controller

Robot performance is highly dependent on properly tuned constants. Start with conservative values and increase gradually while testing on your actual track.

| Parameter | Role | Starting Suggestion |
|---|---|---|
| `baseSpeed` | Default forward speed | `80–120` (out of 255) |
| `Kp` | Proportional gain | Increase until oscillation appears, then reduce slightly |
| `Kd` | Derivative gain | Increase to suppress oscillation caused by Kp |
| `maxSpeed` | Speed ceiling | `180–220` — leave headroom for correction |
| `minSpeed` | Speed floor | `0` or slightly negative for sharp turns |

**Recommended starting point:**
```cpp
lineFollow(readLine(), 100, 200, 0, 0.05, 0.8, 0);
```

> Tune on your actual competition track. Lighting, surface reflectivity, and track color will all affect sensor readings and may require recalibration of `minimum[]` and `maximum[]` arrays.

---


This project is open-source and available under the [MIT License](LICENSE).  
Feel free to use, modify, and build upon this work — just give credit where it's due.

---

> ⭐ If this project was helpful or inspired your own build, consider giving it a star on GitHub!
