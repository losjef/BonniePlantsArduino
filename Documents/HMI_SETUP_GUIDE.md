# CM5-T15W HMI Setup & Communication Guide

Configuration guide for connecting and programming the AutomationDirect
CM5-T15W 15″ C-more HMI touchscreen to the BonnieConveyor P1AM-100
controller.

---

## 1. Communication Overview

```
┌────────────────────┐         Ethernet          ┌────────────────────┐
│    CM5-T15W HMI    │◄──────────────────────────►│   P1AM-100 CPU     │
│  (Modbus TCP       │    TCP port 502            │  (Modbus TCP       │
│   Client)          │                            │   Server)          │
└────────────────────┘                            └────────────────────┘
                                                   │ SPI backplane
                                            ┌──────┼──────┐
                                            ▼      ▼      ▼
                                        P1-16ND3 P1-16TR P1-08DAL-2
                                        (Inputs) (Relays) (0-10 V)
```

- **Protocol:** Modbus TCP (MBAP over TCP/IP)
- **Transport:** Ethernet, single cable between HMI and P1AM-ETH shield
- **Connection:** HMI is the **client**; P1AM is the **server** (slave)
- **Port:** 502 (standard Modbus TCP)
- **Unit ID:** 1 (default, configurable in firmware)
- **Function Codes Used:**
  - FC 03 — Read Holding Registers (HMI reads status & calibration)
  - FC 06 — Write Single Register (HMI writes commands & calibration)
  - FC 02 — Read Discrete Inputs (optional: raw physical input status)
- **Byte Order:** Big-endian (Modbus standard)

---

## 2. C-more Programming Software Setup

Open **C-more Programming Software** and configure as follows.

### 2.1 Create/Open Project

1. **File → New Project**
2. Select panel type: **EA9-T15CL** (CM5-T15W equivalent in C-more)
3. Set orientation: Landscape

### 2.2 Add Modbus TCP Protocol

1. **Project → Protocol**
2. Click **Add Protocol**
3. Select: **Modbus TCP/IP (Client)**
4. Click **OK**

### 2.3 Configure Device Connection

| Setting | Value |
|---------|-------|
| Device Name | `P1AM` (or any descriptive name) |
| IP Address | `192.168.1.100` |
| Port | `502` |
| Unit ID (Slave Address) | `1` |
| Response Timeout | `1000 ms` |
| Retry Count | `3` |
| Polling Interval | `250 ms` (adjust as needed) |

### 2.4 Configure HMI Network

1. **Panel → Ethernet Settings**
2. Set the HMI to a static IP on the same subnet:

| Parameter | Value |
|-----------|-------|
| HMI IP Address | `192.168.1.200` (example) |
| Subnet Mask | `255.255.255.0` |
| Gateway | `192.168.1.1` |

> **Important:** The HMI and the P1AM must be on the same subnet.
> If your plant network uses a different addressing scheme, update
> `Config.h` in the firmware and rebuild.

---

## 3. Modbus Register Map

All data exchange between the HMI and the P1AM controller uses
**Holding Registers** (4xxxxx addressing in C-more).

C-more uses **1-based** Modbus addressing by default.  The table below
shows both the 0-based protocol address (as defined in firmware) and the
corresponding C-more tag address.

### 3.1 Status Registers (Read by HMI)

These are written by the Arduino every scan cycle (~20 ms).
Configure the HMI to **read** these registers on a polling interval.

| 0-based | C-more Tag | Name | Data Type | Description |
|:-------:|:----------:|------|-----------|-------------|
| 0 | 400001 | System State | UINT16 | 0=Stop, 1=Run1, 2=TimeDelay, 3=Run2, 4=EStop, 5=BuzzerDelay |
| 1 | 400002 | Speed Selected | UINT16 | Currently active speed (1–6, 0=none) |
| 2 | 400003 | Remaining Time | UINT16 | Time-delay countdown, seconds |
| 3 | 400004 | Wait Time | UINT16 | Configured delay duration, seconds |
| 4 | 400005 | Batch Counter Lo | UINT16 | Current batch count, low 16 bits |
| 5 | 400006 | Batch Counter Hi | UINT16 | Current batch count, high 16 bits |
| 6 | 400007 | Total Counter Lo | UINT16 | Lifetime count, low 16 bits |
| 7 | 400008 | Total Counter Hi | UINT16 | Lifetime count, high 16 bits |
| 8 | 400009 | Selected Tray | UINT16 | Currently active tray (1–6, 0=none) |
| 9 | 400010 | Heartbeat | UINT16 | Toggles 0/1 each second (comm watchdog) |
| 10 | 400011 | Input State | UINT16 | Raw P1-16ND3 bitmask (bit 0 = ch 1) |
| 11 | 400012 | Output State | UINT16 | Raw P1-16TR bitmask (bit 0 = ch 1) |

**32-bit counters:** Batch Count and Total Count span two registers each
(low word + high word).  In C-more, configure as a **DINT** (32-bit
integer) starting at the low-word address:

- Batch Counter: **DINT at 400005** (registers 400005–400006)
- Total Counter: **DINT at 400007** (registers 400007–400008)

### 3.2 Command Registers (Written by HMI)

The HMI writes to these registers to send operator commands.
**One-shot** registers are cleared to 0 by the Arduino after processing
— the HMI should write the value and does not need to clear it.

| 0-based | C-more Tag | Name | Data Type | Description |
|:-------:|:----------:|------|-----------|-------------|
| 100 | 400101 | Command | UINT16 | One-shot command code (see table below) |
| 101 | 400102 | Speed Select | UINT16 | Speed setting 1–6 (**persistent** — not cleared) |
| 102 | 400103 | Tray Select | UINT16 | Tray setting 1–6 (**persistent** — not cleared) |
| 103 | 400104 | Timer Adjust | INT16 | Signed adjustment: +1, -1, +5, -5, etc. (one-shot) |
| 104 | 400105 | Save Timer | UINT16 | Write 1 to save current wait time to flash (one-shot) |
| 105 | 400106 | Reset Total | UINT16 | Write 1 to reset lifetime counter (one-shot) |

**Command codes** for register 400101:

| Value | Action | Valid When |
|:-----:|--------|------------|
| 0 | No command (idle) | — |
| 1 | **Start** | State = Stop, speed and tray selected |
| 2 | **Stop** | Any state except E-Stop |
| 3 | **Start Delay** | State = Run1 or Run2 |
| 4 | **E-Stop** | Any state |
| 5 | **E-Stop Clear** | State = E-Stop |
| 6 | **Reset Batch Counter** | Any state |

### 3.3 Calibration Registers (Read/Write)

Calibration factors are stored as **UINT16** values **scaled ×1000**.
A factor of `1.000` is stored as `1000`; a factor of `0.850` is stored
as `850`; a factor of `1.250` is stored as `1250`.

On the HMI, divide by 1000 for display and multiply by 1000 before
writing.

#### Motor Speed Factors

7 motors × 6 speeds = 42 registers starting at 400201.

| Motor | Name | Speed 1 | Speed 2 | Speed 3 | Speed 4 | Speed 5 | Speed 6 |
|:-----:|------|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|
| 1 | Infeed Belt | 400201 | 400202 | 400203 | 400204 | 400205 | 400206 |
| 2 | Lower Soil Belt | 400207 | 400208 | 400209 | 400210 | 400211 | 400212 |
| 3 | Flat Filler Belt | 400213 | 400214 | 400215 | 400216 | 400217 | 400218 |
| 4 | Planting Line | 400219 | 400220 | 400221 | 400222 | 400223 | 400224 |
| 5 | Motor 5 | 400225 | 400226 | 400227 | 400228 | 400229 | 400230 |
| 6 | Motor 6 | 400231 | 400232 | 400233 | 400234 | 400235 | 400236 |
| 8 | Upper Soil Belt | 400237 | 400238 | 400239 | 400240 | 400241 | 400242 |

**Formula:**  C-more tag = `400001 + 200 + (motorIndex × 6) + speedIndex`
where motorIndex = 0–6, speedIndex = 0–5.

#### Tray Time Factors (seconds per plant count)

6 trays × 6 speeds = 36 registers starting at 400251.

| Tray | Name | Speed 1 | Speed 2 | Speed 3 | Speed 4 | Speed 5 | Speed 6 |
|:----:|------|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|
| 1 | 6-06 | 400251 | 400252 | 400253 | 400254 | 400255 | 400256 |
| 2 | 3.5 | 400257 | 400258 | 400259 | 400260 | 400261 | 400262 |
| 3 | 4.5 | 400263 | 400264 | 400265 | 400266 | 400267 | 400268 |
| 4 | 5 | 400269 | 400270 | 400271 | 400272 | 400273 | 400274 |
| 5 | Gallon | 400275 | 400276 | 400277 | 400278 | 400279 | 400280 |
| 6 | 8 | 400281 | 400282 | 400283 | 400284 | 400285 | 400286 |

**Formula:** C-more tag = `400001 + 250 + (trayIndex × 6) + speedIndex`
where trayIndex = 0–5, speedIndex = 0–5.

#### Tray Motor-8 Speed Factors

6 trays × 6 speeds = 36 registers starting at 400301.

| Tray | Name | Speed 1 | Speed 2 | Speed 3 | Speed 4 | Speed 5 | Speed 6 |
|:----:|------|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|
| 1 | 6-06 | 400301 | 400302 | 400303 | 400304 | 400305 | 400306 |
| 2 | 3.5  | 400307 | 400308 | 400309 | 400310 | 400311 | 400312 |
| 3 | 4.5  | 400313 | 400314 | 400315 | 400316 | 400317 | 400318 |
| 4 | 5    | 400319 | 400320 | 400321 | 400322 | 400323 | 400324 |
| 5 | Gallon | 400325 | 400326 | 400327 | 400328 | 400329 | 400330 |
| 6 | 8    | 400331 | 400332 | 400333 | 400334 | 400335 | 400336 |

**Formula:** C-more tag = `400001 + 300 + (trayIndex × 6) + speedIndex`

#### Save Calibration

| 0-based | C-more Tag | Description |
|:-------:|:----------:|-------------|
| 400 | 400401 | Write 1 to save all calibration data to flash (one-shot) |

---

## 4. Suggested HMI Screen Layout

### Screen 1 — Main Operations

```
┌─────────────────────────────────────────────────────────┐
│  BONNIE CONVEYOR CONTROL                    [●] Online  │
│                                                         │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐              │
│  │  START   │  │  STOP    │  │  DELAY   │              │
│  │  (green) │  │  (red)   │  │  (yellow)│              │
│  └──────────┘  └──────────┘  └──────────┘              │
│                                                         │
│  State:  [ RUN1        ]    Speed: [ 3 ]   Tray: [6-06]│
│                                                         │
│  Batch Count:   [  1234  ]     Timer:  [ 45 ] / [ 60 ] │
│  Total Count:   [ 98765  ]                              │
│                                                         │
│  Speed Select:  [1] [2] [3] [4] [5] [6]                │
│  Tray Select:   [6-06][3.5][4.5][ 5 ][Gal][ 8 ]       │
│                                                         │
│  ┌────────────┐  ┌────────────┐  ┌────────────┐        │
│  │ E-STOP     │  │ TIMER  [+] │  │ CALIBRATION│        │
│  │ (big red)  │  │ ADJUST [-] │  │  SCREEN →  │        │
│  └────────────┘  │ [SAVE]     │  └────────────┘        │
│                  └────────────┘                         │
└─────────────────────────────────────────────────────────┘
```

#### Object-to-Register Mapping

| Screen Object | Type | Register | Notes |
|--------------|------|----------|-------|
| START button | Momentary, writes 1 | 400101 | Command = Start |
| STOP button | Momentary, writes 2 | 400101 | Command = Stop |
| DELAY button | Momentary, writes 3 | 400101 | Command = StartDelay |
| E-STOP button | Momentary, writes 4 | 400101 | Command = EStop |
| State display | Numeric/text | 400001 | Map 0–5 to state names |
| Speed display | Numeric | 400002 | Read-only |
| Tray display | Numeric | 400009 | Map 1–6 to tray names |
| Batch Count | DINT numeric | 400005 | 32-bit, read-only |
| Total Count | DINT numeric | 400007 | 32-bit, read-only |
| Remaining Time | Numeric | 400003 | Read-only, seconds |
| Wait Time | Numeric | 400004 | Read-only, seconds |
| Speed 1–6 buttons | Set value, writes 1–6 | 400102 | Persistent |
| Tray 1–6 buttons | Set value, writes 1–6 | 400103 | Persistent |
| Timer [+] button | Momentary, writes +1 | 400104 | One-shot (signed) |
| Timer [-] button | Momentary, writes -1 | 400104 | One-shot (signed) |
| Timer [SAVE] | Momentary, writes 1 | 400105 | One-shot |
| Online indicator | Bit lamp | 400010 | Heartbeat — blink = OK |
| Reset Batch | Momentary, writes 6 | 400101 | Command = ResetCurrent |
| Reset Total | Momentary, writes 1 | 400106 | One-shot |

### Screen 2 — Calibration

A grid or table screen for editing motor speed factors, tray time
factors, and tray motor-8 factors.

| Object | Register Range | Display |
|--------|---------------|---------|
| Motor factor grid (7×6) | 400201–400242 | Show as value/1000 (e.g., "1.000") |
| Tray time factor grid (6×6) | 400251–400286 | Show as value/1000 |
| Tray motor-8 factor grid (6×6) | 400301–400336 | Show as value/1000 |
| Save Calibration button | 400401 | Momentary, writes 1 |

> **Tip:** Use a **numeric entry** object with min=0, max=9999. Display
> with 3 implied decimal places for the ×1000 scaling.

---

## 5. State Indicator Configuration

Use a **multi-state indicator** or **text list** mapped to register
400001 to display the current system state:

| Register Value | Display Text | Suggested Color |
|:--------------:|-------------|:---------------:|
| 0 | STOPPED | Red background |
| 1 | RUNNING | Green background |
| 2 | TIME DELAY | Yellow background |
| 3 | RUNNING | Green background |
| 4 | **E-STOP** | Flashing red background |
| 5 | STARTING... | Yellow background |

Use **visibility triggers** on the START/STOP/DELAY buttons:

| Button | Visible When (400001) |
|--------|----------------------|
| START | = 0 (Stopped) |
| STOP | = 1, 2, or 3 (any running state) |
| DELAY | = 1 or 3 (Run1 or Run2) |
| E-STOP CLEAR | = 4 (E-Stop active) |

---

## 6. Communication Health Monitoring

### Heartbeat Watchdog

Register **400010** (Heartbeat) toggles between 0 and 1 every second.
Use this to detect communication loss:

1. Create a **1-second timer** on the HMI
2. On each tick, compare the current heartbeat value to the previous
3. If the value has not changed for 5 consecutive seconds, display a
   **COMM LOST** alarm

### Input/Output Mirrors

Registers **400011** (Input State) and **400012** (Output State) contain
the raw bitmasks from the P1-16ND3 and P1-16TR modules.  These can be
used for a **diagnostics screen** showing real-time I/O status:

**Input State (400011) bit mapping:**

| Bit | Indicator | Meaning when 1 | Meaning when 0 |
|:---:|-----------|----------------|----------------|
| 0 | Start btn | Pressed | Released |
| 1 | Stop btn | Normal (NC closed) | Pressed (NC opened) |
| 2 | Delay btn | Pressed | Released |
| 3 | E-Stop | Normal (NC closed) | Activated (NC opened) |
| 4–15 | Spare | — | — |

**Output State (400012) bit mapping:**

| Bit | Indicator | Meaning when 1 |
|:---:|-----------|----------------|
| 0 | Motor 1 relay | ON — Infeed Belt |
| 1 | Motor 2 relay | ON — Lower Soil Belt |
| 2 | Motor 3 relay | ON — Flat Filler Belt |
| 3 | Motor 4 relay | ON — Planting Line |
| 4 | Motor 5 relay | ON |
| 5 | Motor 6 relay | ON |
| 7 | Motor 8 relay | ON — Upper Soil Belt |
| 8 | Red light | ON |
| 9 | Yellow light | ON |
| 10 | Green light | ON |
| 11 | Buzzer | ON |
| 12 | E-Stop relay | ON |

---

## 7. Polling Configuration

Recommended C-more polling groups to balance responsiveness with
network load:

| Group | Registers | Interval | Purpose |
|-------|-----------|----------|---------|
| Fast Status | 400001–400012 (12 regs) | 250 ms | State, counters, timer |
| Commands | 400101–400106 (6 regs) | On-write-only | HMI writes only when operator acts |
| Calibration | 400201–400342 (142 regs) | 2000 ms or on-screen-open | Only needed on calibration screen |
| Heartbeat | 400010 (1 reg) | 1000 ms | Communication health check |

> **Tip:** In C-more, set the fast-status read as the default poll and
> put the calibration read inside a **screen-trigger** so it only polls
> when the calibration screen is active.

---

## 8. Wiring — Ethernet Connection

| From | To | Cable |
|------|----|-------|
| CM5-T15W Ethernet port (RJ45) | Network switch | Cat 5e or better |
| P1AM-ETH shield Ethernet port (RJ45) | Same network switch | Cat 5e or better |

If connecting directly (no switch), use a **crossover cable** or rely on
auto-MDI/MDI-X (both the W5500 on the P1AM-ETH and the CM5-T15W
support auto-crossover).

---

## 9. Troubleshooting

| Symptom | Check |
|---------|-------|
| HMI shows "No Communication" | Verify IP addresses are on the same subnet; ping P1AM from a PC on the same network |
| Heartbeat not toggling | Arduino may not be running; check USB serial for boot messages |
| Commands not responding | Verify C-more is writing to register 400101, not 40101 (C-more is 1-based, 6-digit) |
| Counters show wrong values | Ensure 32-bit counter is configured as **DINT** spanning two consecutive registers |
| Calibration values look wrong | Remember ×1000 scaling: a displayed `1.250` should be written as integer `1250` |
| Only one HMI can connect | The Arduino accepts one TCP client at a time — additional connections are refused |
| Registers read as 0 after power cycle | Calibration is loaded from flash on boot and pushed to registers; if flash is empty, factory defaults are used |
