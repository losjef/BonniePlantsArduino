# BonnieConveyor — P1AM Arduino Controller

Automated conveyor-line controller for plant production, running on an
AutomationDirect Productivity1000 I/O platform with a P1AM-100 Arduino
CPU and a CM5-T15W C-more HMI touchscreen.

---

## Architecture Shift

### Original System (QtVersion/)

| Layer | Technology |
|-------|-----------|
| CPU / Runtime | Windows PC running Qt 6.6.3 desktop application |
| I/O | Waveshare Modbus RTU modules (Relay 16CH, Input, Analog 8CH) |
| Communication | Two RS-485 serial COM ports, 57600 baud 8N1, three Modbus device IDs |
| Operator UI | Qt Widgets GUI (mainwindow.ui) on the PC monitor |
| Persistence | JSON files on the PC filesystem |

### New System (this project)

| Layer | Technology |
|-------|-----------|
| CPU / Runtime | P1AM-100 (SAMD21 Arduino MKR) in a P1-01AC base — runs a single Arduino sketch |
| I/O | P1-16ND3, P1-16TR, P1-08DAL-2 — accessed directly over the SPI backplane via the P1AM library |
| Communication | Ethernet (P1AM-ETH shield, W5500) — Modbus TCP **server** on port 502 |
| Operator UI | CM5-T15W 15″ C-more HMI touchscreen — connects as a Modbus TCP **client** |
| Persistence | SAMD21 on-chip flash via the FlashStorage library |

**Key difference:** the P1AM-100 talks to the I/O modules through the
Productivity1000 backplane (SPI), _not_ through Modbus.  Modbus TCP is
used only for the HMI ↔ controller data exchange.  This eliminates the
PC, the serial adapters, and the multi-device-ID complexity of the
original design.

---

## Bill of Materials

| Part | Qty | Description |
|------|:---:|-------------|
| P1AM-100 | 1 | Arduino MKR-format CPU (SAMD21) for ProductivityOpen |
| P1AM-ETH | 1 | Ethernet shield (WIZnet W5500) for P1AM-100 |
| P1-01AC | 1 | AC power base unit |
| P1-16ND3 | 1 | 16-point DC input module (Slot 1) |
| P1-16TR | 1 | 16-point relay output module (Slot 2) |
| P1-08DAL-2 | 1 | 8-channel 0–10 V analog output, 12-bit DAC (Slot 3) |
| ZL-RTB20 | 3 | Removable terminal blocks |
| ZL-P1-CBL18 | 2 | 18-pin I/O cables |
| ZL-P1-CBL10 | 1 | 10-pin I/O cable |
| CM5-T15W | 1 | 15″ C-more HMI touchscreen |

---

## Project Structure

```
BonnieConversionTest/
├── platformio.ini          PlatformIO build configuration
├── include/
│   └── Config.h            Hardware config, register map, calibration structs
├── src/
│   └── main.cpp            Complete Arduino sketch (setup/loop, state machine,
│                            I/O, timers, Modbus TCP server, flash persistence)
├── QtVersion/              Original Qt desktop application (reference only)
└── README.md               This file
```

Only **two source files** make up the entire firmware:

- **`include/Config.h`** — the single source of truth for every tunable
  constant: slot assignments, I/O channel maps, Modbus register
  addresses, state definitions, calibration data structures, factory
  defaults, and network settings.

- **`src/main.cpp`** — the Arduino sketch containing `setup()`,
  `loop()`, the state machine, physical I/O reads/writes, timer
  handling, Modbus TCP server logic, calibration persistence, and
  serial production logging.

---

## Key Design Decisions

### 1. Direct Backplane I/O (P1AM library)

The P1AM library communicates with Productivity1000 I/O modules over
the SPI backplane — the same bus the native P1-622 CPU uses.  All
digital and analog I/O is accessed with three functions:

```cpp
uint32_t inputs = P1.readDiscrete(SLOT_DI);       // P1-16ND3
P1.writeDiscrete(SLOT_DO, outputBitmask);          // P1-16TR
P1.writeAnalog(SLOT_AO, channel, dacValue);        // P1-08DAL-2
```

This is simpler and faster than the original Modbus RTU approach
(no serial framing, no device addressing, no request/reply latency).

### 2. Modbus TCP Server for HMI

The ArduinoModbus library (which depends on ArduinoRS485) creates a
Modbus TCP server on port 502.  The CM5-T15W C-more HMI connects as
a client and reads/writes holding registers.

The register map is divided into three blocks:

| Block | Registers | Direction | Purpose |
|-------|-----------|-----------|---------|
| Status | 0–11 | Arduino → HMI | State, speed, counters, timers, heartbeat, I/O mirrors |
| Commands | 100–105 | HMI → Arduino | Start/stop/e-stop, speed/tray select, timer adjust |
| Calibration | 200–401 | Both | Motor factors, tray time factors, tray motor-8 factors |

One-shot commands (start, stop, timer adjust) are cleared by the
Arduino after processing.  Persistent selections (speed, tray) remain
in their registers until changed.

Calibration factors are transported as `uint16` values scaled ×1000
(e.g., factor 1.250 → register value 1250).

### 3. Deterministic Scan Cycle

The `loop()` function runs a fixed-order scan:

1. Pet the hardware watchdog
2. Handle Modbus TCP (accept connections, poll for requests)
3. Scan physical inputs (edge detection)
4. Process HMI command registers
5. Handle countdown, counter, and buzzer timers
6. Write physical outputs (relays + analog)
7. Update status registers
8. Enforce 20 ms minimum cycle time

All timing uses `millis()`-based comparisons — no interrupts, no
callbacks, no RTOS.  The 20 ms floor is configured in
`Config.h` (`SCAN_CYCLE_MS`).

### 4. State Machine

Six states, matching the original Qt application:

```
STOP ──(start)──→ BUZZER_DELAY ──(3 s)──→ RUN1
                                            │
                              (delay btn) ──┘
                                            ↓
                    RUN2 ←──(timer done)── TIME_DELAY
                      │                     ↑
                      └──── (delay btn) ────┘

Any state ──(e-stop)──→ ESTOP ──(clear)──→ BUZZER_DELAY → prior state
Any state (except ESTOP) ──(stop)──→ STOP
```

- **STOP** — all motors off, red light, counter saved to flash
- **BUZZER_DELAY** — 3-second audible warning before starting
- **RUN1** — all motors on at selected speed, green light, counter ticking
- **TIME_DELAY** — motors 1/2/8 off, remaining motors run, yellow light, countdown timer
- **RUN2** — all motors on, counter reset and ticking, green light
- **ESTOP** — immediate all-off, red light, e-stop relay energized

### 5. Motor Speed Control

Each motor has 6 speed calibration factors (one per speed setting).
The output voltage is:

```
DAC value = percentToDAC( BASE_SPEED × motorFactor[speedIndex] )
```

Motor 8 (Upper Soil Belt) uses a per-tray factor instead of its own
motor factor, since its speed depends on the tray being run:

```
DAC value = percentToDAC( BASE_SPEED × trayMotor8Factor[trayIndex][speedIndex] )
```

The P1-08DAL-2 has a 12-bit DAC (0–4095 = 0.0–10.0 V).

### 6. Counter System

- **Timer-based counting** — each plant count increments on a
  `millis()` interval derived from the tray's time factor for the
  selected speed.  There is no physical sensor.
- **Batch counter** resets on each time-delay completion.
- **Total counter** persists in SAMD flash, batched every 100 counts
  to reduce flash wear.

### 7. Flash Persistence (FlashStorage)

Two separate `FlashStorage` objects minimize write wear:

| Object | Data | Written when |
|--------|------|-------------|
| `flashCalib` | `CalibrationData` (motor/tray factors, wait time) | HMI "Save Calibration" command |
| `flashCounter` | `CounterData` (total lifetime count) | Every 100 plant counts, on stop, on e-stop |

A magic number (`0xBEEFCAFE`) guards against reading uninitialized
flash — if absent, factory defaults are loaded.

### 8. Hardware Watchdog

`P1.configWD(5000, TOGGLE)` sets a 5-second watchdog.  If `P1.petWD()`
is not called within 5 seconds (firmware hang), the base forces all
outputs to a safe off state.

### 9. Production Logging

Production runs are logged as CSV over the serial port:

```
LOG,<millis>,<tray_name>,<speed>,<batch_count>,<total_count>
```

Connect a serial terminal or data logger at 115200 baud to capture
production history.

---

## Wiring Summary

### P1-16ND3 Discrete Inputs (Slot 1)

| Channel | Function | Type |
|:-------:|----------|------|
| 1 | Start button | NO (rising edge) |
| 2 | Stop button | NC (falling edge = pressed) |
| 3 | Start Delay button | NO (rising edge) |
| 4 | E-Stop | NC (falling edge = activated) |
| 5–16 | Available | — |

### P1-16TR Relay Outputs (Slot 2)

| Channel | Function |
|:-------:|----------|
| 1 | Motor 1 — Infeed Belt |
| 2 | Motor 2 — Lower Soil Belt |
| 3 | Motor 3 — Flat Filler Belt |
| 4 | Motor 4 — Planting Line Belt |
| 5 | Motor 5 |
| 6 | Motor 6 |
| 7 | (unused) |
| 8 | Motor 8 — Upper Soil Belt |
| 9 | Red indicator light |
| 10 | Yellow indicator light |
| 11 | Green indicator light |
| 12 | Buzzer |
| 13 | E-Stop output relay |
| 14–16 | Available |

### P1-08DAL-2 Analog Outputs (Slot 3)

| Channel | Function | Range |
|:-------:|----------|-------|
| 1 | Motor 1 speed | 0–10 V |
| 2 | Motor 2 speed | 0–10 V |
| 3 | Motor 3 speed | 0–10 V |
| 4 | Motor 4 speed | 0–10 V |
| 5 | Motor 5 speed | 0–10 V |
| 6 | Motor 6 speed | 0–10 V |
| 7 | (unused) | — |
| 8 | Motor 8 speed | 0–10 V |

---

## Network Configuration

| Parameter | Default | Where to change |
|-----------|---------|-----------------|
| IP Address | 192.168.1.100 | `Config.h` → `DEFAULT_IP` |
| Subnet | 255.255.255.0 | `Config.h` → `DEFAULT_SUBNET` |
| Gateway | 192.168.1.1 | `Config.h` → `DEFAULT_GATEWAY` |
| Modbus TCP Port | 502 | `Config.h` → `MODBUS_TCP_PORT` |
| MAC Address | 60:52:D0:06:70:27 | `Config.h` → `DEFAULT_MAC` (match your P1AM-ETH sticker) |

The CM5-T15W HMI should be configured to connect to this IP on port 502
as a Modbus TCP client.

---

## Building and Uploading

### PlatformIO (VS Code)

```bash
# Build
pio run

# Upload
pio run --target upload

# Serial monitor
pio device monitor --baud 115200
```

### Arduino IDE

1. Install board support: **Tools → Board Manager → Arduino SAMD Boards**
2. Install libraries: P1AM, ArduinoModbus, ArduinoRS485, Ethernet, FlashStorage
3. Select board: **Arduino MKR Zero** (P1AM-100 compatible)
4. Open `src/main.cpp`, copy into a new sketch, and place `Config.h` in the sketch folder
5. Upload

---

## Modbus Register Map (for HMI Programming)

### Status Registers (read by HMI)

| Register | Content | Values |
|:--------:|---------|--------|
| 0 | System state | 0=Stop, 1=Run1, 2=TimeDelay, 3=Run2, 4=EStop, 5=BuzzerDelay |
| 1 | Speed selected | 1–6 (0 = none) |
| 2 | Remaining time | Countdown seconds |
| 3 | Wait time | Configured delay (seconds) |
| 4 | Current counter (low) | Batch count, low 16 bits |
| 5 | Current counter (high) | Batch count, high 16 bits |
| 6 | Total counter (low) | Lifetime count, low 16 bits |
| 7 | Total counter (high) | Lifetime count, high 16 bits |
| 8 | Selected tray | 1–6 (0 = none) |
| 9 | Heartbeat | Toggles 0/1 each second |
| 10 | Input state | Raw P1-16ND3 bitmask |
| 11 | Output state | Raw P1-16TR bitmask |

### Command Registers (written by HMI)

| Register | Content | Values |
|:--------:|---------|--------|
| 100 | Command (one-shot) | 0=None, 1=Start, 2=Stop, 3=StartDelay, 4=EStop, 5=EStopClear, 6=ResetCurrent |
| 101 | Speed select | 1–6 (persistent) |
| 102 | Tray select | 1–6 (persistent) |
| 103 | Timer adjust (one-shot) | Signed int16 (+1, -1, +5, -5, etc.) |
| 104 | Save timer (one-shot) | 1 = save current wait time to flash |
| 105 | Reset total counter (one-shot) | 1 = reset lifetime counter |

### Calibration Registers (read/write)

| Range | Content | Encoding |
|-------|---------|----------|
| 200–241 | Motor speed factors (7 motors × 6 speeds) | Factor × 1000 (1000 = 1.000) |
| 250–285 | Tray time factors (6 trays × 6 speeds) | Factor × 1000 |
| 300–335 | Tray motor-8 factors (6 trays × 6 speeds) | Factor × 1000 |
| 400 | Save calibration (one-shot) | 1 = save all calibration to flash |
