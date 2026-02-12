# BonnieConveyor

Automated conveyor-line controller for plant production using AutomationDirect P1AM-100 Arduino CPU with Productivity1000 I/O modules and CM5-T15W C-more HMI.

## Features

- **Direct I/O Control** — P1AM library for backplane SPI communication with P1-16ND3 (inputs), P1-16TR (relays), P1-08DAL-2 (0-10V analog)
- **Modbus TCP Server** — Ethernet-based HMI communication via P1AM-ETH shield (WIZnet W5500)
- **Six-State Machine** — Stop, Run1, TimeDelay, Run2, E-Stop, BuzzerDelay with safety interlocks
- **Multi-Speed/Multi-Tray** — 6 speeds × 6 tray types with per-motor and per-tray calibration factors
- **Flash Persistence** — Calibration and lifetime counters stored in SAMD21 flash (FlashStorage)
- **Hardware Watchdog** — 5-second timeout with safe-state outputs
- **Production Logging** — CSV serial output for batch tracking

## Hardware

| Component | Part Number | Qty | Description |
|-----------|-------------|:---:|-------------|
| CPU | P1AM-100 | 1 | Arduino MKR-based controller (SAMD21) |
| Ethernet | P1AM-ETH | 1 | Ethernet shield (WIZnet W5500) |
| Base | P1-01AC | 1 | AC power supply base |
| Inputs | P1-16ND3 | 1 | 16-point DC input module (Slot 1) |
| Relays | P1-16TR | 1 | 16-point relay output (Slot 2) |
| Analog | P1-08DAL-2 | 1 | 8-channel 0-10V output, 12-bit DAC (Slot 3) |
| HMI | CM5-T15W | 1 | 15" C-more touchscreen |
| Cables | ZL-RTB20, ZL-P1-CBL18, ZL-P1-CBL10 | 6 | Terminal blocks and I/O cables |

## Quick Start

### PlatformIO

```bash
git clone https://github.com/YOUR_USERNAME/BonnieConveyor.git
cd BonnieConveyor
pio run --target upload
pio device monitor --baud 115200
```

### Arduino IDE

1. Install libraries: `P1AM`, `ArduinoModbus`, `ArduinoRS485`, `Ethernet`, `FlashStorage`
2. Select board: **Arduino MKR Zero**
3. Open `src/main.cpp` and upload

### Network Configuration

Edit `include/Config.h` to match your plant network:

```cpp
constexpr uint8_t DEFAULT_IP[4]      = { 192, 168, 1, 100 };
constexpr uint8_t DEFAULT_SUBNET[4]  = { 255, 255, 255, 0 };
constexpr uint8_t DEFAULT_GATEWAY[4] = { 192, 168, 1, 1 };
```

Update the MAC address to match your P1AM-ETH shield sticker.

## Documentation

- [README.md](README.md) — Architecture, design decisions, wiring
- [Documents/HMI_SETUP_GUIDE.md](Documents/HMI_SETUP_GUIDE.md) — CM5-T15W setup, Modbus register map, screen layouts
- [include/Config.h](include/Config.h) — Hardware configuration reference

## Repository Structure

```
BonnieConveyor/
├── platformio.ini          # PlatformIO build config
├── include/
│   └── Config.h            # Hardware config, register map, calibration
├── src/
│   └── main.cpp            # Arduino sketch (state machine, I/O, Modbus server)
├── Documents/
│   └── HMI_SETUP_GUIDE.md  # C-more HMI configuration guide
├── QtVersion/              # Legacy Qt desktop application (reference only)
└── README.md               # Full project documentation
```

## Modbus Register Map (Quick Reference)

| Register | Function | R/W |
|:--------:|----------|:---:|
| 0–11 | Status (state, counters, timers, heartbeat) | R |
| 100–105 | Commands (start/stop, speed/tray select, adjustments) | W |
| 200–241 | Motor speed calibration factors (×1000) | R/W |
| 250–285 | Tray time factors (×1000) | R/W |
| 300–335 | Tray motor-8 factors (×1000) | R/W |
| 400 | Save calibration to flash | W |

See [HMI_SETUP_GUIDE.md](Documents/HMI_SETUP_GUIDE.md) for complete register details.

## License

MIT License — see [LICENSE](LICENSE) file for details.

## Author

Jeff L. — Plant automation project, February 2026
