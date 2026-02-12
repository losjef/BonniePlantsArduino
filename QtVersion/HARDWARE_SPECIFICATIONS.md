# Hardware Specifications - Conveyor Interface

## Document Information
- **Created**: December 22, 2025
- **Hardware Source**: [Waveshare Modbus RTU Relay (D)](https://www.waveshare.com/wiki/Modbus_RTU_Relay_(D))
- **Purpose**: Industrial conveyor control system with 8 motors + auxiliary outputs

---

## System Overview

This system uses Waveshare Modbus RTU modules for industrial I/O control:
- **Digital Outputs**: 16-channel relay module (motors, lights, buzzer, e-stop)
- **Digital Inputs**: Relay module configured as input-only (buttons and sensors)  
- **Analog Outputs**: 8-channel 0-10V output module (motor speed control)

### Communication
- **Protocol**: Modbus RTU
- **Baud Rate**: 57600 (8N1 - 8 data bits, No parity, 1 stop bit)
- **Interface**: RS485 serial bus
- **Port Configuration**: 
  - Output devices: COM4 (default)
  - Input devices: COM5 (default)

---

## Waveshare Modbus RTU Relay 16CH - Digital Outputs

**Module**: [Waveshare Modbus RTU Relay 16CH](https://www.waveshare.com/wiki/Modbus_RTU_Relay_16CH)

### Module Specifications
| Parameter | Value |
|-----------|-------|
| Power Supply | DC 7-36V (12V or 24V recommended) |
| Communication | RS485 (Modbus RTU) |
| Baud Rate Support | 4800, 9600, 19200, 38400, **57600**, 115200, 128000, 256000 |
| Default Format | 9600, N, 8, 1 (factory setting) |
| Relay Channels | **16 Channels** (0x0000-0x000F) |
| Contact Form | 1NO 1NC (Normally Open + Normally Closed) |
| Contact Rating | ≤10A 250V AC or ≤10A 30V DC |
| Protocol | Standard Modbus RTU |
| Address Range | 1-255 (device ID) |
| Power Consumption | 0.26W standby, 4.7W all relays on (@ 5V) |

### Safety Specifications
- **Load Matching**: Must not exceed 10A 250V AC or 10A 30V DC
- **Inductive Loads**: Require RC snubber circuits or varistors for arc suppression
- **Protection**: Built-in power isolation, magnetic isolation, TVS protection
- **Environment**: Not for use in humid, high-temp, flammable, or dusty environments

---

## Digital Output Address Mapping

Current implementation uses **ONE Waveshare Modbus RTU Relay 16CH module** configured as **Device ID 1**, providing 16 relay channels (0-15). The system uses 13 of these channels.

### Motor Control Relays (Channels 0-7)

| Relay Channel | Modbus Address | Device ID | Function | Physical Relay | Notes |
|--------------|----------------|-----------|----------|----------------|-------|
| 0 | 0x0000 | 1 | Motor 1 ON/OFF | Relay 1 | Infeed Belt |
| 1 | 0x0001 | 1 | Motor 2 ON/OFF | Relay 2 | Lower Soil Belt |
| 2 | 0x0002 | 1 | Motor 3 ON/OFF | Relay 3 | Flat Filler Belt |
| 3 | 0x0003 | 1 | Motor 4 ON/OFF | Relay 4 | Planting Line Belt |
| 4 | 0x0004 | 1 | Motor 5 ON/OFF | Relay 5 | Future/Reserved |
| 5 | 0x0005 | 1 | Motor 6 ON/OFF | Relay 6 | Future/Reserved |
| 6 | 0x0006 | 1 | *Not Used* | Relay 7 | Available |
| 7 | 0x0007 | 1 | Motor 8 ON/OFF | Relay 8 | Upper Soil Belt |

**Function**: Control VFD enable/disable (motor on/off). Relays switch power to VFD run inputs.

### Auxiliary Outputs (Channels 8-12)

| Relay Channel | Modbus Address | Device ID | Function | Physical Relay | Notes |
|--------------|----------------|-----------|----------|----------------|-------|
| 8 | 0x0008 | 1 | Red Light | Relay 9 | E-stop indicator |
| 9 | 0x0009 | 1 | Yellow Light | Relay 10 | Caution/delay indicator |
| 10 | 0x000A | 1 | Green Light | Relay 11 | Running indicator |
| 11 | 0x000B | 1 | Buzzer | Relay 12 | Countdown/alert buzzer |
| 12 | 0x000C | 1 | E-Stop Output | Relay 13 | E-stop relay control |
| 13-15 | 0x000D-0x000F | 1 | *Available* | Relays 14-16 | Future expansion |

**Function**: Stack light control (red/yellow/green), audible alarm (buzzer), and E-stop output signal.

### Device Addressing

The 16-channel relay module uses **Device ID 1** on the RS485 bus. All 16 channels (0x0000-0x000F) are addressed on this single module. The software sends all digital writes to Device ID 1, with the channel number (0-15) in the address field selecting the specific relay.

**Code Implementation**:
```cpp
// All digital outputs use Device ID 1
const int m_digitalOutAddress{ 1 };

// Channel addresses differentiate modules
m_motor1ModbusDigitalDeviceID{ 1 };     // Module 1, Channel 0
m_redLightModbusDigitalDeviceID{ 1 };   // Module 2, Channel 8 (same Device ID)
```

**Modbus Command Example**:
```
// Turn ON Motor 1 (Module 1, Channel 0)
Send: 01 05 00 00 FF 00 8C 3A
      |  |  |____|
      |  |    +--- Address 0x0000 (Channel 0)
      |  +-------- Function 05 (Write Single Coil)
      +----------- Device ID 1

// Turn ON Red Light (Module 2, Channel 8)
Send: 01 05 00 08 FF 00 8D FA
      |  |  |____|
      |  |    +--- Address 0x0008 (Channel 8)
      |  +-------- Function 05
      +----------- Device ID 1 (same as motors)
```

---

## Digital Input Module - Waveshare Relay with Digital Inputs

**Configuration**: Second Waveshare relay module (likely Modbus RTU Relay 8CH (D)) used **exclusively for digital inputs**. Relay outputs on this module are not used.

**Module**: Waveshare Modbus RTU Relay (D) or similar with 8 digital input channels

### Input Specifications
| Parameter | Value |
|-----------|-------|
| Device ID | 2 (configured) |
| Function | Digital input reading only |
| Input Channels | 8 DI (Digital Inputs) |
## Digital Input Address Mapping

Current implementation uses **Device ID 2** for digital inputs (second relay module used as input-only device).
2. **NPN (Active Low)**: COM terminal → positive power supply (5-36V DC)
3. **PNP (Active High)**: COM terminal → negative power supply

**Current System**: Uses dry contact or NPN configuration for push buttons.

**Response Time**: ~75ms including Modbus command send/receive

| Input Channel | Modbus Address | Device ID | Function | Input Terminal | Wiring |
|--------------|----------------|-----------|----------|----------------|--------|
| 0 | 0x0000 | 2 | Start Button | Terminal 1 | Dry contact/NPN |
| 1 | 0x0001 | 2 | Stop Button | Terminal 2 | Dry contact/NPN |
| 2 | 0x0002 | 2 | Start Delay Button | Terminal 3 | Dry contact/NPN |
| 3 | 0x0003 | 2 | E-Stop Button | Terminal 4 | Dry contact/NPN |
| 4-7 | 0x0004-0x0007 | 2 | *Available* | Terminals 5-8 | Future sensors |

### Input Wiring Configuration
The Waveshare module supports three input modes:
1. **Dry Contact (Passive)**: COM terminal = NC (not connected)
2. **NPN (Active Low)**: COM terminal → positive power supply (5-36V DC)
3. **PNP (Active High)**: COM terminal → negative power supply

**Current System**: Appears to use dry contact or NPN configuration for buttons.

**Response Time**: ~75ms including command send/receive (per Waveshare specs)

---

## Analog Output Specification - Waveshare Modbus RTU Analog Output 8CH (B)

**Module**: [Waveshare Modbus RTU Analog Output 8CH (B)](https://www.waveshare.com/wiki/Modbus_RTU_Analog_Output_8CH_(B))

### Module Specifications
| Parameter | Value |
|-----------|-------|
| Output Type | Voltage (0-10V DC) |
| Channels | 8 Channels |
| Output Range | 0-10000 mV (0-10V) |
| Resolution | 1 mV (millivolt precision) |
| Max Output Current | 100 mA per channel |
| Communication | RS485 (Modbus RTU) |
| Baud Rate Support | 4800, 9600, 19200, 38400, **57600**, 115200, 128000, 256000 |
| Default Format | 9600, N, 8, 1 (factory setting) |
| Protocol | Standard Modbus RTU |
| Address Range | 1-255 (device ID) |
| Power Supply | Input voltage must exceed output voltage + 5V margin |
| Power Consumption | 0.39W @ 5V standby |

**CRITICAL**: Input power supply must be ≥15V DC to achieve 10V output (minimum 5V headroom required).

### Voltage Output Mapping (VFD Speed Control)

Current implementation uses **Device ID 3** for analog outputs.

| Channel | Modbus Address | Device ID | Function | Output Range | Motor | Terminal |
|---------|----------------|-----------|----------|--------------|-------|----------|
| 0 | 0x0000 | 3 | Motor 1 Speed | 0-10V DC | Infeed Belt VFD | AO1 |
| 1 | 0x0001 | 3 | Motor 2 Speed | 0-10V DC | Lower Soil Belt VFD | AO2 |
| 2 | 0x0002 | 3 | Motor 3 Speed | 0-10V DC | Flat Filler Belt VFD | AO3 |
| 3 | 0x0003 | 3 | Motor 4 Speed | 0-10V DC | Planting Line Belt VFD | AO4 |
| 4 | 0x0004 | 3 | Motor 5 Speed | 0-10V DC | Future Motor VFD | AO5 |
| 5 | 0x0005 | 3 | Motor 6 Speed | 0-10V DC | Future Motor VFD | AO6 |
| 6 | 0x0006 | 3 | *Not Used* | - | - | AO7 |
| 7 | 0x0007 | 3 | Motor 8 Speed | 0-10V DC | Upper Soil Belt VFD | AO8 |

### Analog Output Protocol Details

**Function Codes**:
- `0x03`: Read holding registers (read current output values)
- `0x06`: Write single register (set one channel)
- `0x10`: Write multiple registers (set multiple channels)

**Register Map**:
| Address | Content | Read/Write | Function Code |
|---------|---------|------------|---------------|
| 0x0000-0x0007 | Output voltage (mV) for channels 1-8 | R/W | 0x03, 0x06, 0x10 |
| 0x2000 | UART parameters (baud/parity) | R/W | 0x03, 0x06 |
| 0x4000 | Device address (1-255) | R/W | 0x03, 0x06 |
| 0x8000 | Software version | R | 0x03 |

**Value Format**: Voltage in **millivolts (mV)**, big-endian (high byte first)
- 0x0000 = 0 mV = 0V (motor stopped)
- 0x03E8 = 1000 mV = 1V
- 0x1388 = 5000 mV = 5V (50% speed typical)
- 0x2710 = 10000 mV = 10V (100% speed)

---

## Modbus Protocol Details (Waveshare)

### Supported Function Codes
| Function Code | Description | Used By System |
|--------------|-------------|----------------|
| 01 | Read Coil Status (relay status) | ✓ Read relay states |
| 02 | Read Input Status | ✓ Read button states |
| 03 | Read Holding Registers | ✓ Read configuration |
| 05 | Write Single Coil | ✓ Control single relay |
| 06 | Write Single Register | Configure device |
| 0F | Write Multiple Coils | ✓ Control multiple relays |
| 10 | Write Multiple Registers | Configure multiple settings |

### Register Address Map (Waveshare Relay Module)

| Address (HEX) | Content | Permission | Function Code |
|--------------|---------|------------|---------------|
| 0x0000-0x0007 | Relay Channels 1-8 | Read/Write | 0x01, 0x05, 0x0F |
| 0x00FF | Control All Relays | Write | 0x05 |
| 0x0200-0x0207 | Flash ON Delay | Write | 0x05 |
| 0x0400-0x0407 | Flash OFF Delay | Write | 0x05 |
| 1x0000-1x0007 | Input Channels 1-8 | Read | 0x02 |
| 4x1000-4x1007 | Relay Control Modes | Read/Write | 0x03, 0x06, 0x10 |
| 4x2000 | UART Parameters | Read/Write | 0x03, 0x06 |
| 4x4000 | Device Address | Read/Write | 0x03, 0x06 |
| 4x8000 | Software Version | Read | 0x03 |

### Relay Control Modes
| Mode Value | Mode Name | Behavior |
|-----------|-----------|----------|
| 0x0000 | Normal Mode | Relay controlled by commands only (default) |
| 0x0001 | Linkage Mode | Relay follows input channel state |
| 0x0002 | Toggle Mode | Relay toggles on input pulse |
| 0x0003 | Edge Trigger | Relay changes on input edge |

**Current System**: Uses Normal Mode (0x0000) - relays controlled by software commands only.

---

## Command Examples (Waveshare Format)

### Digital Relay Commands (Device 1)

#### Control Single Relay
**Turn ON Motor 1 (Relay 0)**
```
Send:    01 05 00 00 FF 00 8C 3A
Receive: 01 05 00 00 FF 00 8C 3A
```

**Turn OFF Motor 1 (Relay 0)**
```
Send:    01 05 00 00 00 00 CD CA
Receive: 01 05 00 00 00 00 CD CA
```

#### Read Relay Status
**Read all 8 relay states**
```
Send:    01 01 00 00 00 08 3D CC
Receive: 01 01 01 00 51 88          // All OFF
Receive: 01 01 01 FF 91 88          // All ON
Receive: 01 01 01 0F D1 89          // Relays 0-3 ON, 4-7 OFF
```

#### Read Input Status (Device 2)
**Read all 8 input channels**
```
Send:    02 02 00 00 00 08 78 3C   // Note: Device ID 2 for inputs
Receive: 02 02 01 00 A0 98          // All inputs low
Receive: 02 02 01 01 61 58          // Input 0 triggered
Receive: 02 02 01 09 60 9B          // Inputs 0 and 3 triggered
```

#### Write Multiple Relays
**Turn ON Motors 1-4 (Relays 0-3), OFF Motors 5,6,8 (Relays 4-7)**
```
Send:    01 0F 00 00 00 08 01 0F FE 95
Receive: 01 0F 00 00 00 08 54 0C
```

---

### Analog Output Commands (Device 3)

#### Modbus Device Configuration
- [ ] Confirm **Waveshare Modbus RTU Relay 16CH** set to Device ID = 1
  - [ ] All 16 channels (0-15) functional on single module
- [ ] Verify **second Waveshare relay module** set to Device ID = 2 (input-only use)
  - [ ] Digital inputs configured (8 channels)
  - [ ] Relay outputs not wired (inputs only)
- [ ] Verify Waveshare Analog Output 8CH (B) Device ID = 3
- [ ] Test RS485 baud rate set to 57600 on all 3 devices (factory default is 9600)
- [ ] Verify RS485 A-A, B-B connections (not swapped across all modules)
- [ ] Check 120Ω termination resistors on RS485 bus ends (first and last device only)

#### Power Supply Requirements
- [ ] Relay module (Device 1): 7-36V DC (12V or 24V recommended) - 4.7W max @ 5V
- [ ] Input module (Device 2): 7-36V DC (powers digital input optocouplers only)
- [ ] Analog output module (Device 3): **Minimum 15V DC** to achieve 10V output (headroom required)
- [ ] Verify analog module input voltage ≥ (max output voltage + 5V)
- [ ] Input button wiring: 5-36V DC power for active inputs, or dry contact switches

#### Safety Systems
- [ ] Test E-stop circuit hard-wires relay power off (not just software control)
- [ ] Verify all 8 motor relays drop out on E-stop activation
- [ ] Check red/yellow/green stack light wiring and addressing
- [ ] Test buzzer activation on countdown timer

#### VFD Integration
- [ ] Verify each VFD analog input accepts 0-10V (not 0-5V or 4-20mA)
- [ ] Check VFD parameter: analog input = speed reference (not other function)
- [ ] Test VFD minimum speed threshold (e.g., 0.5V = start, 0V = stop)
- [ ] Verify VFD maximum speed limit (10V should = rated motor RPM)
- [ ] Check relay contact ratings (10A max) vs. VFD input current draw

#### Electrical Safety
- [ ] Install RC snubber circuits on inductive loads (motor contactors)
- [ ] Verify fuse/breaker sizing for 10A relay contacts
- [ ] Check wire gauge for current capacity (motors, VFDs)
- [ ] Inspect all terminal connections for tightness
- [ ] Test ground continuity on all metal enclosures

### Device Configuration Commands
**Set Baud Rate to 57600** (if not factory-set) - send to each device:
```
Send: 00 06 20 00 00 04 82 09
```

**Set Device Address to 1** (both relay modules):
```
Send: 00 06 40 00 00 01 5C 1B
```

**Set Device Address to 2** (digital input module):
```
Send: 00 06 40 00 00 02 1C 1A
```

**Set Device Address to 3** (analog output module):
```
Send: 00 06 40 00 00 03 DD DA
```

**Read Device Software Version** (works on all modules):
```
Send:    00 03 80 00 00 01 AC 1B
Receive: 00 03 02 00 C8 84 12       // Relay version 2.00
Receive: 00 03 02 00 64 B9 AF       // Analog version 1.00
```

**Test Analog Output** - Set all channels to 5V for testing:
```
Send: 03 10 00 00 00 08 10 13 88 13 88 13 88 13 88 13 88 13 88 13 88 13 88 [CRC]
```
(Use multimeter to verify 5V on each AO terminal)

**Test Relay Modules** - Verify channel separation:
```
// Test Module 1, Channel 0 (Motor 1)
Send: 01 05 00 00 FF 00 8C 3A

// Test Module 2, Channel 8 (Red Light)
Send: 01 05 00 08 FF 00 8D FA

// Both should respond - confirms same Device ID, different channels
```
**Set Motor 8 to 10V (100% speed)**
```
Send:    03 06 00 07 27 10 [CRC]
         |  |  |     |
         |  |  |     +-- 0x2710 = 10000 mV = 10V
         |  |  +-------- Channel 7 (Motor 8)
         |  +----------- Function 06
         +-------------- Device ID 3

Actual:  03 06 00 07 27 10 78 D3
Receive: 03 06 00 07 27 10 78 D3
```

#### Set Multiple Channel Outputs
**Set all 8 motors to 7.5V (75% speed)**
```
Send:    03 10 00 00 00 08 10 1D 4C 1D 4C 1D 4C 1D 4C 1D 4C 1D 4C 1D 4C 1D 4C [CRC]
         |  |  |     |     |  |________________Repeat 0x1D4C (7500mV) for 8 channels
         |  |  |     |     +--- 0x10 = 16 bytes (8 channels × 2 bytes)
         |  |  |     +--------- 8 registers
         |  |  +--------------- Start at channel 0
         |  +------------------ Function 10 (write multiple registers)
         +--------------------- Device ID 3

Receive: 03 10 00 00 00 08 [CRC]
```

#### Read Channel Output Values
**Read all 8 channel current outputs**
```
Send:    03 03 00 00 00 08 45 FE
Receive: 03 03 10 13 88 13 88 13 88 13 88 00 00 00 00 00 00 1D 4C [CRC]
         |  |  |  |_____|_____|_____|_____|_____|_____|_____|______|
         |  |  |    CH0   CH1   CH2   CH3   CH4   CH5   CH6   CH7
         |  |  |  (5V)  (5V)  (5V)  (5V)  (0V)  (0V)  (0V)  (7.5V)
         |  |  +--- 0x10 = 16 bytes returned
         |  +------ Function 03
         +--------- Device ID 3
```

---

## Hardware Configuration Checklist

### Pre-Deployment Verification
- [ ] Confirm Waveshare Modbus RTU Relay (D) Device ID = 1
- [ ] Verify digital input module Device ID = 2
- [ ] Verify analog output module model and Device ID = 3
- [ ] Test RS485 baud rate set to 57600 on all devices
- [ ] Verify RS485 A-A, B-B connections (not swapped)
- [ ] Check 120Ω termination resistors on RS485 bus ends
- [ ] Confirm power supply 7-36V DC (12V or 24V recommended)
- [ ] Test E-stop circuit isolates all relay outputs
- [ ] Verify VFD analog input accepts 0-10V (not 0-5V or 4-20mA)
- [ ] Check relay contact ratings vs. actual load current
- [ ] Install snubber circuits on inductive loads (motors)

### Device Configuration Commands
**Set Baud Rate to 57600** (if not factory-set):
```
Send: 00 06 20 00 00 04 82 09
```

**Set Device Address to 1** (digital output module):
```
Send: 00 06 40 00 00 01 5C 1B
```

**Set Device Address to 2** (digital input module):
```
Send: 00 06 40 00 00 02 1C 1A
```

**Set Device Address to 3** (analog output module):
```
Send: 00 06 40 00 00 03 DD DA
```

**Read Device Software Version**:
```Implementation Notes for Software

### Current Code Addresses (Verified Against Waveshare Specs)

**Digital Outputs (Device 1)** ✓ CORRECT
```cpp
// Motors 1-6, 8 using channels 0-7 on relay module
m_motor1DigitalOutAddress = 0;  // Relay channel 0 ✓
m_motor2DigitalOutAddress = 1;  // Relay channel 1 ✓
// ... up to motor8 = 7 ✓
m_digitalOutAddress = 1;        // Device ID ✓
```

**Digital Inputs (Device 2)** ✓ CORRECT (assumed)
```cpp
startButtonAddress = 0;         // Input channel 0 ✓
stopButtonAddress = 1;          // Input channel 1 ✓
startDelayButtonAddress = 2;    // Input channel 2 ✓
eStopButtonAddress = 3;         // Input channel 3 ✓
m_digitalInAddress = 2;         // Device ID (verify with hardware)
```

**Analog Outputs (Device 3)** ✓ CORRECT
```cpp
// Motors using channels 0-7 on analog output module
m_motor1AnalogOutAddress = 0;   // Channel 0 (0x0000) ✓
m_motor2AnalogOutAddress = 1;   // Channel 1 (0x0001) ✓
// ... up to motor8 = 7 ✓
m_analogOutAddress = 3;         // Device ID ✓

// Value format: millivolts (0-10000)
// Software must convert percentage to mV:
// speed_percent = 75  →  voltage_mV = 75% × 10000 = 7500 mV
```

**Stack Lights/Buzzer/E-Stop (Addresses 8-12)** ✓ CORRECT
```cpp
// These addresses are VALID on 16-channel relay module (channels 0-15)
m_redLightDigitalOutAddress = 8;     // ✓ Relay 9 (channel 8)
m_yellowLightDigitalOutAddress = 9;  // ✓ Relay 10 (channel 9)
m_greenLightDigitalOutAddress = 10;  // ✓ Relay 11 (channel 10)
m_buzzerDigitalOutAddress = 11;      // ✓ Relay 12 (channel 11)
m_EStopOutDigitalOutAddress = 12;    // ✓ Relay 13 (channel 12)
m_digitalOutAddress = 1;             // ✓ Device ID 1

### Waveshare Product Pages
- [Modbus RTU Relay (D) - Digital I/O](https://www.waveshare.com/wiki/Modbus_RTU_Relay_(D))
- [Modbus RTU Analog Output 8CH (B) - 0-10V Outputs](https://www.waveshare.com/wiki/Modbus_RTU_Analog_Output_8CH_(B))

### Technical Documentation
- [Relay Module Development Protocol V2](https://www.waveshare.com/wiki/Modbus_RTU_Relay_(D)#Development_Protocol_V2)
- [Analog Output Development Protocol V2](https://www.waveshare.com/wiki/Modbus_RTU_Analog_Output_8CH_(B)#Development_Protocol_V2)
- [Modbus Protocol Specification](https://www.waveshare.com/wiki/Modbus_Protocol_Specification)
- [Waveshare BootLoader Documentation](https://www.waveshare.com/wiki/Modbus_Series_BootLoader_Description)

### Downloadable Resources
- [Relay Module Demo Code](https://files.waveshare.com/wiki/Modbus-RTU-Relay-(D)/Modbus_RTU_Relay_D_Code.zip) (Python, STM32, Arduino, PLC)
- [Analog Output Demo Code](https://files.waveshare.com/wiki/Modbus-RTU-Analog-Output-8CH/Modbus_RTU_Analog_Output_Code.zip)
- [SSCOM Serial Debugger](https://files.waveshare.com/upload/b/b3/Sscom5.13.1.zip)
- [Modbus Poll Software](https://www.modbustools.com/download.html)

---

## Revision History

| Date | Version | Changes | Author |
|------|---------|---------|--------|
| 2025-12-22 | 1.0 | Initial hardware documentation from Waveshare relay specs | GitHub Copilot |
| 2025-12-22 | 1.1 | Added Waveshare Analog Output 8CH (B) specifications, command examples | GitHub Copilot |
| 2025-12-22 | 1.2 | **Corrected to Waveshare Modbus RTU Relay 16CH** (single module, not two 8-channel modules) | GitHub Copilot |
| 2025-12-22 | 1.3 | Clarified input module: Second Waveshare relay module used for digital inputs only (relays not wired) | GitHub Copilot |
```cpp
// Current implementation should convert speed percentage to millivolts
int voltage_mV = static_cast<int>((percent / 100.0) * 10000);
// 0% → 0 mV, 50% → 5000 mV, 100% → 10000 mV
```

**Update test mode** - Mock analog values in mV:
```cpp
qDebug() << "[TEST MODE] Analog Write - Motor:" << motorAddress 
         << "Voltage:" << voltage_mV << "mV (" << percent << "%)";
```

---

## Next Steps

1. ✓ **Analog output module specifications obtained** - Waveshare Modbus RTU Analog Output 8CH (B) confirmed
2. ✓ **Relay module confirmed** - Waveshare Modbus RTU Relay 16CH on Device ID 1 (user verified)
3. ✓ **Input module confirmed** - Second Waveshare relay module used as input-only on Device ID 2 (user verified)
4. **Physical wiring diagram** - Create based on actual terminal connections
5. **Device ID verification** - Confirm actual Device IDs match code:
   - Device 1: 16-channel relay module (addresses 0x0000-0x000F)
   - Device 2: Relay module for inputs only (addresses 0x0000-0x0007)
   - Device 3: Analog output module (addresses 0x0000-0x0007)
6. **Voltage conversion check** - Verify writeanalogoutput.cpp converts % to mV correctly
7. **Power supply** - Ensure ≥15V DC for analog module to output 10V
8. **Input module relay isolation** - Verify relay outputs on Device 2 are not wired (input-only use)
9. **Hardware test plan execution** - Follow HARDWARE_TEST_PLAN.md with actual devices
6. **Power supply** - Ensure ≥15V DC for analog module to output 10V
7. **Relay module addressing** - Verify both modules respond to Device ID 1 with proper channel separation
8. **Hardware test plan execution** - Follow HARDWARE_TEST_PLAN.md with actual devices
## Safety and Reliability Notes

### Relay Contact Protection
1. **Inductive Loads** (motors, solenoids): MUST use RC snubber circuits or MOVs to suppress arcing
2. **Maximum Current**: 10A per contact - verify actual motor contactor current
3. **Switching Frequency**: Minimize rapid on/off cycles to extend relay life
4. **Contact Inspection**: Regularly inspect for pitting, welding, or wear

### E-Stop Circuit Requirements
- E-stop button MUST cut power to all relay coils (fail-safe design)
- Software E-stop state machine provides secondary safety layer
- All motor relays de-energize on E-stop (0 = OFF, relays drop out)
- Red light activates, green/yellow lights deactivate on E-stop

### RS485 Communication
- **Maximum Bus Length**: 1200m (4000 ft) at 57600 baud
- **Termination**: 120Ω resistors at both ends of bus (critical for reliability)
- **Isolation**: Waveshare modules include magnetic isolation for noise immunity
- **Error Handling**: Software implements CRC16 checking, retries on communication errors

### Environment Ratings
⚠️ **NOT suitable for**:
- Humid environments (condensation risk)
- High temperature (>50°C operating temperature)
- Flammable/explosive atmospheres (not ATEX rated)
- Dusty environments without enclosure (IP20 rating typical)

**Recommended**: Install in clean, climate-controlled electrical cabinet with proper ventilation.

---

## Code Implementation Notes

### Current Address Scheme
```cpp
// Device IDs (mainwindow.h)
const int m_digitalOutAddress{ 1 };    // Relay module
const int m_digitalInAddress{ 2 };     // Input module  
const int m_analogOutAddress{ 3 };     // Analog output module

// Motor digital outputs (Relay channels on Device 1)
const quint16 m_motor1DigitalOutAddress{ 0 };  // Relay 0
const quint16 m_motor2DigitalOutAddress{ 1 };  // Relay 1
// ... up to motor8 on Relay 7

// Motor analog outputs (Channels on Device 3)
const quint16 m_motor1AnalogOutAddress{ 0 };   // Channel 0
const quint16 m_motor2AnalogOutAddress{ 1 };   // Channel 1
// ... up to motor8 on Channel 7
```

### TODO: Verify in Hardware
1. **Stack Light Addresses**: Currently `m_redLightDigitalOutAddress{ 8 }` etc.
   - Waveshare relay module only has 8 channels (0-7)
   - These must be on a **second relay module** or different device
   - **ACTION**: Confirm second module Device ID and update code

2. **Analog Output Module**: Code assumes Device ID 3
   - **ACTION**: Provide exact model number of analog output module
   - **ACTION**: Verify register addresses match Modbus specifications

3. **Modbus Baud Rate**: Code uses 57600, Waveshare default is 9600
   - **ACTION**: Confirm all devices reconfigured to 57600 before deployment

---

## References

- [Waveshare Modbus RTU Relay (D) Wiki](https://www.waveshare.com/wiki/Modbus_RTU_Relay_(D))
- [Waveshare Development Protocol V2](https://www.waveshare.com/wiki/Modbus_RTU_Relay_(D)#Development_Protocol_V2)
- [Modbus Protocol Specification](https://www.waveshare.com/wiki/Modbus_Protocol_Specification)
- [Waveshare BootLoader Documentation](https://www.waveshare.com/wiki/Modbus_Series_BootLoader_Description)

---

## Revision History

| Date | Version | Changes | Author |
|------|---------|---------|--------|
| 2025-12-22 | 1.0 | Initial hardware documentation from Waveshare specs | GitHub Copilot |

---

## Next Steps

1. **Obtain analog output module specifications** - Provide URL or manual
2. **Verify second relay module** for stack lights/buzzer (outputs 8-12)
3. **Physical wiring diagram** - Create based on actual terminal connections
4. **Hardware test plan execution** - Follow HARDWARE_TEST_PLAN.md with actual devices
5. **Update code TODOs** - Replace placeholder addresses with confirmed values
