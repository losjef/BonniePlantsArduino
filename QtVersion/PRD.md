# Product Requirements Document (PRD)
## Bonnie Plant Conveyor Interface System

**Version:** 1.0  
**Date:** December 22, 2025  
**Project:** ConveyorInterfaceQt  
**Platform:** Windows, Qt 6.6.3  

---

## 1. Executive Summary

The Bonnie Plant Conveyor Interface System is a Qt-based industrial control application designed to manage and control a multi-motor conveyor belt system for plant production operations. The system provides precise motor speed control, tray-specific configurations, production counting, timing operations, and safety features through a touch-optimized user interface.

---

## 2. Product Overview

### 2.1 Purpose
To provide operators with a reliable, user-friendly interface for controlling conveyor belt systems used in plant production, enabling:
- Multi-speed motor operation with calibrated speed profiles
- Tray-specific production configurations
- Production counting and tracking
- Timed operation modes
- Emergency stop safety features
- Real-time monitoring and control

### 2.2 Target Users
- Production line operators
- Plant production facility supervisors
- Maintenance personnel
- Production managers

### 2.3 Operating Environment
- **Hardware:** Industrial PC with touch screen display
- **Operating System:** Windows 10/11
- **Communication:** Modbus RTU over RS-485 serial communication
- **I/O Hardware:** 
  - Waveshare Modbus RTU Relay 16CH (Device ID 1) - Digital outputs
  - Waveshare Relay Module (Device ID 2) - Digital inputs only
  - Waveshare Modbus RTU Analog Output 8CH (B) (Device ID 3) - 0-10V motor speed control
- **Test Mode:** Hardware-free simulator for development and testing
- **Documentation:** 
  - HARDWARE_SPECIFICATIONS.md - Complete hardware details
  - TEST_MODE_DOCUMENTATION.md - Simulator usage guide

---

## 3. System Architecture

### 3.1 Communication Protocols
- **Modbus RTU Serial Client**: 57600 baud, 8 data bits, no parity, 1 stop bit
- **Input Scanning**: Separate thread for continuous digital input monitoring
- **Output Control**: Modbus write operations for digital outputs and analog outputs (0-10V)

### 3.2 Core Components

#### Motors (8 Units)
- Independent speed control (0-100% via 0-10V analog output)
- Individual digital on/off control
- Calibration factors for each of 6 speed settings
- Modbus addressable (separate device IDs for digital and analog control)

#### Trays (6 Configurations)
- Tray-specific speed calibration factors for each motor
- Time factor calibration for production counting
- Motor 8 (Upper Soil Belt) has tray-specific calibration

#### Counters
- **Total Counter**: Persistent lifetime production count
- **Current Counter**: Session-based production count with reset capability

---

## 4. Functional Requirements

### 4.1 Operating States

#### 4.1.1 Stop State
- **Function**: All motors stopped, system idle
- **UI Display**: System ready for operation
- **Entry Conditions**: User presses Stop button, power-on initialization, or E-stop cleared
- **Exit Conditions**: User initiates Run1 or TimeDelay mode

#### 4.1.2 Run1 State
- **Function**: Continuous operation at selected speed
- **Motor Control**: All motors running at calibrated speeds based on selected speed (1-6) and active tray
- **Production Counting**: Active, increments based on time intervals
- **Entry Conditions**: User presses Start button
- **Exit Conditions**: User presses Stop button or initiates TimeDelay mode

#### 4.1.3 Time Delay State
- **Function**: Countdown timer before transitioning to Run2
- **Countdown Display**: Real-time countdown from configured wait time
- **Motor Control**: Motors continue running during countdown
- **Entry Conditions**: User presses Start Timer button
- **Exit Conditions**: Timer reaches zero (auto-transition to Run2) or user stops

#### 4.1.4 Run2 State
- **Function**: Continued operation after time delay
- **Motor Control**: Motors running, counter updates more frequently
- **Production Counting**: Enhanced counting mode
- **Entry Conditions**: Automatic transition from TimeDelay state
- **Exit Conditions**: User presses Stop button or E-stop activated

#### 4.1.5 E-Stop State
- **Function**: Emergency shutdown
- **Motor Control**: All motors immediately stopped
- **Safety**: All digital outputs turned off
- **Entry Conditions**: E-stop input signal detected
- **Exit Conditions**: E-stop cleared, transitions to Buzzer Delay state
- **Priority**: Highest priority, overrides all other states

#### 4.1.6 Buzzer Delay State
- **Function**: Brief delay with buzzer activation after E-stop cleared
- **Duration**: Configurable delay (default: short duration)
- **Buzzer**: Active during delay
- **Entry Conditions**: E-stop condition cleared
- **Exit Conditions**: Delay timer expires, returns to previous operating state

### 4.2 Speed Control

#### 4.2.1 Speed Selection
- **Speed Levels**: 6 discrete speed settings (Speed 1 through Speed 6)
- **Selection Method**: Touch button interface
- **Visual Feedback**: Active speed button highlighted
- **Change Timing**: Can be changed during operation (applies immediately)

#### 4.2.2 Speed Calibration
- **Motor Speed Factors**: Each motor has 6 calibration factors (one per speed)
- **Tray Time Factors**: Each tray has 6 time factors affecting production counting
- **Calculation**: `Actual Speed = Base Speed (100%) × Motor Calibration Factor × Tray Factor`
- **Persistence**: Calibration data stored in JSON files

### 4.3 Tray Management

#### 4.3.1 Tray Selection
- **Available Trays**: 6 tray configurations
- **Selection Method**: Touch button interface
- **Active Indication**: Selected tray highlighted
- **Effect**: Changes speed calibration and timing for all motors

#### 4.3.2 Tray Configuration
- **Speed Time Factors**: 6 factors per tray (one per speed setting)
- **Motor 8 Calibration**: Unique calibration factors for Upper Soil Belt motor
- **Configuration Access**: Admin-level calibration dialogs

### 4.4 Production Counting

**TIMER-BASED SYSTEM**: Plant counting uses calibrated timers, not physical sensors per plant.

#### 4.4.1 Total Counter
- **Scope**: Lifetime production count across all sessions
- **Persistence**: Saved to file (counter.txt)
- **Display**: LCD-style numerical display
- **Reset**: No reset capability (audit trail)

#### 4.4.2 Current Counter
- **Scope**: Session-based production count
- **Increment Logic**: Timer-based, interval calculated from speed and tray factors
- **Display**: LCD-style numerical display
- **Reset**: Manual reset via UI button

#### 4.4.3 Counting Algorithm (Timer-Based)
- **Method**: QTimer triggers count increments at calculated intervals
- **Interval Formula**: Each tray has 6 speed-specific time factors (seconds between plants)
  - Example: Tray 1, Speed 3 = 0.5 seconds → 2 plants/second
  - Example: Tray 2, Speed 6 = 0.2 seconds → 5 plants/second
- **Calibration**: Time factors configured via Tray Calibration Factors dialog
- **Increment**: Both totalCounter and currentCounter increment on each timer tick
- **State Dependency**: Timer active only in Run2 state
- **Thread Safety**: Mutex-protected counter increments
- **Persistence**: Batched writes to disk (every 10 counts) + immediate save on Stop/E-stop

### 4.5 Timer Operations

#### 4.5.1 Countdown Timer
- **Purpose**: Delay between Run1 and Run2 states
- **Range**: User-adjustable via +/- buttons
- **Increment**: ±1 second per button press
- **Display**: Real-time countdown in seconds
- **Persistence**: Timer setting saved to JSON file

#### 4.5.2 Timer Adjustment
- **Plus Button**: Increase wait time by 1 second
- **Minus Button**: Decrease wait time by 1 second
- **Minimum**: 0 seconds
- **Maximum**: No defined limit (practical limit based on production needs)
- **Save**: Dedicated Save button to persist changes

### 4.6 Configuration Management

#### 4.6.1 Motor Calibration
- **Access**: Motor Factors dialog (admin access)
- **Parameters**: 6 speed calibration factors per motor (8 motors)
- **Storage**: MotorCalibFact.json
- **Format**: JSON with motor names and calibration arrays
- **Validation**: Values typically 0.0 to 2.0 (percentage multipliers)

#### 4.6.2 Tray Calibration
- **Access**: Tray Calib Factors dialog
- **Parameters**: 6 time factors per tray (6 trays)
- **Storage**: TrayFactors.json
- **Effect**: Modifies production counting intervals

#### 4.6.3 Upper Soil Belt Calibration
- **Access**: Upper Soil Belt Factors dialog
- **Parameters**: Tray-specific calibration for Motor 8
- **Storage**: UpperSoilBeltFactors.json
- **Application**: Applied when tray is selected

#### 4.6.4 COM Port Configuration
- **Access**: Update COM Ports dialog
- **Parameters**: 
  - Input COM port (digital input scanning)
  - Output COM port (Modbus output control)
- **Storage**: COMPorts.json
- **Application**: Requires application restart

### 4.7 Safety Features

#### 4.7.1 Emergency Stop
- **Input**: Dedicated E-stop digital input (monitored continuously)
- **Response Time**: Immediate (< 100ms)
- **Action**: All motors stopped, all outputs off
- **Override**: Cannot be overridden by software
- **Recovery**: Buzzer delay state before resuming

#### 4.7.2 Startup Safety
- **Power-On State**: Stop state (all motors off)
- **Output Initialization**: All outputs turned off on startup
- **Modbus Connection**: Verified before operations permitted

---

## 5. User Interface Requirements

### 5.1 Display Configuration
- **Window Mode**: Maximized (includes taskbar)
- **Alternative**: Full screen mode (available but disabled by default)
- **Orientation**: Landscape
- **Resolution**: Optimized for industrial touch panels

### 5.2 Main Screen Elements

#### 5.2.1 Speed Selection Area
- **Buttons**: 6 speed buttons (Speed 1 through Speed 6)
- **Visual State**: Selected button highlighted
- **Layout**: Prominent positioning for easy access

#### 5.2.2 Tray Selection Area
- **Buttons**: 6 tray buttons
- **Visual State**: Selected tray highlighted
- **Layout**: Grouped for quick selection

#### 5.2.3 Control Buttons
- **Start**: Begin continuous operation (Run1)
- **Stop**: Halt all operations
- **Start Timer**: Begin countdown to Run2
- **Visual Feedback**: Button states indicate system status

#### 5.2.4 Counter Displays
- **Total Counter LCD**: Large numerical display, non-resettable
- **Current Counter LCD**: Large numerical display
- **Reset Button**: Resets current counter only
- **Label**: Clear indication of counter type

#### 5.2.5 Timer Controls
- **Timer Display**: Large numerical display showing seconds
- **Plus Button**: Increment timer setting
- **Minus Button**: Decrement timer setting
- **Save Button**: Persist timer configuration
- **Enabled State**: Save button only enabled when timer value changes

#### 5.2.6 Configuration Access
- **Menu Items/Buttons**:
  - Motor Factors
  - Tray Calibration Factors
  - Update COM Ports
  - Upper Soil Belt Factors
  - View Production Log

#### 5.2.7 Production Logging
- **Auto-Logging**: System automatically logs all Run2 production cycles
- **Data Captured**:
  - Timestamp (date and time)
  - Plant count for the run
  - Speed setting (1-6)
  - Tray type selected
  - Total production count since last reset
- **CSV Format**: Production data stored in `ProductionLog.csv`
- **Persistent Storage**: Log survives application restarts
- **Viewing**: Accessible via "View Production Log" menu item

### 5.3 Dialog Windows

#### 5.3.1 Production Log Viewer
- **Table Display**: Shows all logged production runs
- **Columns**: Date/Time, Count, Speed, Tray, Total Count
- **Statistics Panel**:
  - Total Runs
  - Total Plants Processed
  - Average Plants/Run
- **Export Button**: Export logs to CSV file (user-selectable location)
- **Clear Button**: Clear all log entries (with confirmation dialog)
- **Refresh Button**: Update display with latest data
- **Auto-sizing**: Table columns automatically adjust to content

#### 5.3.2 Motor Factors Dialog
- **Input Fields**: 6 calibration factors per motor
- **Motor Labels**: Clear identification (Motor 1-8)
- **Save/Cancel**: Persist or discard changes
- **Validation**: Numerical input validation

#### 5.3.3 Tray Calibration Dialog
- **Input Fields**: 6 time factors per tray
- **Tray Labels**: Tray 1-6 identification
- **Save/Cancel**: Standard dialog controls

#### 5.3.4 COM Ports Dialog
- **Port Selection**: Dropdown or text input for COM port names
- **Input Port**: Digital input scanning port
- **Output Port**: Modbus output control port
- **Validation**: Port name format validation
- **Restart Notice**: Inform user that restart is required

#### 5.3.5 Upper Soil Belt Factors Dialog
- **Tray-Specific**: Factors for each tray configuration
- **Motor 8 Only**: Specific to upper soil belt motor
- **Input Fields**: 6 factors per tray (6 trays)

### 5.4 Touch Optimization
- **Button Size**: Minimum 40x40 pixels for touch targets
- **Spacing**: Adequate spacing between interactive elements
- **Feedback**: Visual feedback on touch interaction
- **Font Size**: Large, readable fonts for industrial environment

---

## 6. Technical Requirements

### 6.1 Hardware Interfaces

**Reference**: See HARDWARE_SPECIFICATIONS.md for complete hardware documentation including wiring, register maps, and command examples.

#### 6.1.1 Modbus RTU Configuration
- **Protocol**: Modbus RTU over RS-485
- **Baud Rate**: 57600 (factory default 9600 - must reconfigure all modules)
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Device IDs**: 
  - Device 1: Waveshare Modbus RTU Relay 16CH (16 digital outputs)
  - Device 2: Waveshare Relay Module (8 digital inputs, relay outputs not wired)
  - Device 3: Waveshare Analog Output 8CH (B) (0-10V motor speed control)
- **Power Supply**: 
  - Relay modules: 7-36V DC (12V or 24V recommended)
  - Analog module: ≥15V DC required for 10V output

#### 6.1.2 Digital Outputs
- **Module**: Waveshare Modbus RTU Relay 16CH (Device ID 1)
- **Type**: Modbus coils (Function Code 0x05 - Write Single Coil)
- **Channels**: 16 relay channels (0x0000-0x000F)
- **Contact Rating**: 10A 250V AC or 10A 30V DC per relay
- **Function**: Motor on/off control, stack lights, buzzer, E-stop output
- **Motor Addresses**: 
  - Motors 1-5: Relays 0-4 (channels 0x0000-0x0004)
  - Motor 8: Relay 7 (channel 0x0007)
- **Auxiliary Outputs**:
  - Red Light: Relay 8 (channel 0x0008)
  - Yellow Light: Relay 9 (channel 0x0009)
  - Green Light: Relay 10 (channel 0x000A)
  - Buzzer: Relay 11 (channel 0x000B)
  - E-Stop Output: Relay 12 (channel 0x000C)
- **Available**: Relays 5-6, 13-15 (future expansion)
- **States**: On (0xFF00), Off (0x0000)

#### 6.1.3 Analog Outputs
- **Module**: Waveshare Modbus RTU Analog Output 8CH (B) (Device ID 3)
- **Type**: Modbus holding registers (Function Code 0x06 - Write Single Register)
- **Range**: 0-10V DC (0-10000 mV)
- **Resolution**: 1 mV precision
- **Max Current**: 100 mA per channel
- **Function**: VFD motor speed control (0-10V analog input)
- **Motor Addresses**: 
  - Motor 1-6, 8: Channels 0-7 (0x0000-0x0007)
- **Value Format**: Millivolts (mV), big-endian
  - 0x0000 = 0 mV = 0V (motor stopped)
  - 0x1388 = 5000 mV = 5V (50% speed)
  - 0x2710 = 10000 mV = 10V (100% speed)
- **Calculation**: `Value_mV = (Percent / 100) × 10000`
- **Power Requirement**: Input voltage ≥ Output voltage + 5V (≥15V for 10V output)

#### 6.1.4 Digital Inputs
- **Module**: Waveshare Relay Module (Device ID 2) - Input-only configuration
- **Type**: Modbus discrete inputs (Function Code 0x02 - Read Input Status)
- **Channels**: 8 digital input channels (0x0000-0x0007)
- **Input Voltage**: 5-36V DC (NPN/PNP or dry contact)
- **Response Time**: ~75ms (including Modbus command send/receive)
- **Button Addresses**:
  - Start Button: Input 0 (0x0000)
  - Stop Button: Input 1 (0x0001)
  - Start Delay Button: Input 2 (0x0002)
  - E-Stop Button: Input 3 (0x0003)
- **Available**: Inputs 4-7 (future sensors)
- **Scanning Thread**: Dedicated thread for continuous monitoring
- **Scan Rate**: Configurable (typically 100ms intervals)
- **Event-Driven**: Signals emitted on input changes
- **Note**: Relay outputs on this module are NOT wired (inputs only)

### 6.2 Data Persistence

#### 6.2.1 Configuration Files (JSON)
- **MotorCalibFact.json**: Motor calibration factors (8 motors × 6 speeds)
- **TrayFactors.json**: Tray calibration factors (6 trays × 6 speeds)
- **UpperSoilBeltFactors.json**: Motor 8 tray-specific calibration
- **COMPorts.json**: Serial port configuration
- **CountDownTimer.json**: Timer settings

#### 6.2.2 Operational Data
- **counter.txt**: Total production counter (plain text)

#### 6.2.3 File Operations
- **Read**: On application startup
- **Write**: On configuration save, counter increment
- **Location**: Application directory
- **Format**: UTF-8 encoded text

### 6.3 Threading Architecture

#### 6.3.1 Main Thread
- **UI Rendering**: All Qt UI updates
- **Modbus Writes**: Output control operations
- **State Machine**: State management and transitions
- **Timers**: QTimer-based countdown and counter updates

#### 6.3.2 Input Scan Thread
- **Function**: Continuous digital input monitoring
- **Communication**: Signal/slot to main thread
- **Independence**: Runs independently, non-blocking
- **Lifecycle**: Started on app launch, terminated on shutdown

### 6.4 Performance Requirements
- **UI Responsiveness**: < 100ms for button press feedback
- **Emergency Stop Response**: < 100ms from input detection to motor shutdown
- **Modbus Communication**: 100ms delay between read/write operations
- **Counter Update**: Real-time (no perceptible delay)
- **Timer Update**: 1-second resolution with visual update

---

## 7. Data Requirements

### 7.1 Motor Configuration Data Structure
```json
{
  "motors": [
    {
      "name": "Motor 1",
      "speedCalFactors": [1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
    }
  ]
}
```

### 7.2 Tray Configuration Data Structure
```json
{
  "trays": [
    {
      "name": "Tray 1",
      "speedTimeFactors": [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
      "motor8Factors": [1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
    }
  ]
}
```

### 7.3 COM Port Configuration
```json
{
  "inputPort": "COM3",
  "outputPort": "COM4"
}
```

### 7.4 Timer Configuration
```json
{
  "waitTime": 60
}
```

---

## 8. Non-Functional Requirements

### 8.1 Reliability
- **Uptime**: 99.5% availability during production hours
- **MTBF**: Minimum 1000 hours continuous operation
- **Error Recovery**: Graceful handling of Modbus communication errors
- **Data Integrity**: Configuration data protected from corruption

### 8.2 Maintainability
- **Code Organization**: Modular architecture with separation of concerns
- **Configuration**: External JSON files for all calibration data
- **Logging**: Debug logging available for troubleshooting
- **Documentation**: Inline code documentation

### 8.3 Usability
- **Learning Curve**: < 30 minutes for basic operation training
- **Touch Interface**: Optimized for gloved operation
- **Visual Clarity**: High contrast, large fonts for industrial environment
- **Error Messages**: Clear, actionable error notifications

### 8.4 Security
- **Configuration Access**: Calibration dialogs require intentional access
- **Data Protection**: Configuration files in application directory
- **E-Stop**: Hardware-enforced safety (cannot be overridden)

### 8.5 Portability
- **Platform**: Windows 10/11 (64-bit)
- **Qt Version**: Qt 6.6.3 with MinGW compiler
- **Dependencies**: QtWidgets, QtSerialBus modules
- **Build System**: CMake 3.5+

---

## 9. Constraints and Assumptions

### 9.1 Constraints
- **Hardware**: Requires Modbus RTU-compatible I/O modules
- **Platform**: Windows-only (no cross-platform support)
- **Display**: Requires touch screen for optimal operation
- **Serial Ports**: Minimum 2 COM ports (input and output)

### 9.2 Assumptions
- **Modbus Devices**: All I/O modules support Modbus RTU at 57600 baud
- **Power**: Stable power supply with UPS backup
- **Network**: No network connectivity required (standalone operation)
- **Operators**: Basic computer literacy and production line experience
- **Calibration**: Factory pre-calibrated, with periodic recalibration by maintenance

---

## 10. Future Enhancements

### 10.1 Potential Features
- **Data Logging**: CSV export of production data
- **Network Connectivity**: Remote monitoring and control
- **User Authentication**: Role-based access control
- **Recipe Management**: Save/load complete production configurations
- **Trending**: Graphical display of production rates over time
- **Alarms**: Configurable alarm conditions and notifications
- **Multi-Language**: Internationalization support

### 10.2 Hardware Expansion
- **Additional Motors**: Support for > 8 motors
- **Multiple Lines**: Control multiple conveyor lines from single interface
- **Barcode Scanner**: Automatic tray identification
- **HMI Integration**: Integration with existing plant HMI systems

---

## 11. Development and Testing

### 11.1 Test Mode (Simulator)

The system includes a hardware-free test mode for development, testing, and demonstration without requiring Modbus devices.

#### 11.1.1 Enabling Test Mode

**Option 1: Compile-Time (CMake)**
```bash
# In CMakeLists.txt, set:
option(CONVEYOR_TEST_MODE "Enable test mode (no hardware required)" ON)

# Then rebuild:
cmake --build build --config Release
```

**Option 2: Runtime (Environment Variable)**
```bash
# Windows PowerShell:
$env:CONVEYOR_TEST_MODE=1
.\ConveyorInterfaceQt.exe

# Or use LaunchTestMode.bat
```

#### 11.1.2 Test Mode Features
- **Visual Indicators**: "[TEST MODE]" in window title, yellow status bar
- **Simulated Hardware**: All Modbus communication bypassed
- **Mock Operations**: Digital/analog outputs logged but not transmitted
- **UI Simulation Panel**: Buttons to simulate Run1, Stop, E-Stop inputs
- **Full Functionality**: All state machines, timers, counters, and UI features operational

#### 11.1.3 Test Mode Logging
```
[TEST MODE] Digital Write simulated - Address: 0 Value: 1
[TEST MODE] Analog Write simulated - Motor: 0 Speed: 75% Voltage: 7500 mV (7.5V)
```

#### 11.1.4 Use Cases
- Development without hardware access
- Testing state machine logic
- UI/UX validation
- Training and demonstrations
- Regression testing without physical setup

### 11.2 Hardware Testing

See HARDWARE_TEST_PLAN.md for comprehensive hardware validation procedures.

---

## 12. Success Criteria

### 11.1 Technical Success
- ✓ Stable operation for 8+ hours continuous runtime
- ✓ < 0.1% error rate in Modbus communication
- ✓ E-stop response time < 100ms
- ✓ Zero data loss on configuration saves
- ✓ UI responsiveness < 100ms

### 11.2 Operational Success
- ✓ Operators can change speeds and trays without assistance
- ✓ Production counting accuracy within ±1%
- ✓ Zero unintended motor activations
- ✓ Calibration changes persist across restarts
- ✓ E-stop recovery without data loss

### 11.3 User Acceptance
- ✓ Positive operator feedback on usability
- ✓ Reduced training time vs. previous system
- ✓ Decreased operator errors
- ✓ Improved production efficiency

---

## 13. Glossary

- **Modbus RTU**: Serial communication protocol for industrial devices
- **E-Stop**: Emergency Stop - safety mechanism to immediately halt operations
- **Calibration Factor**: Multiplier applied to base speed for fine-tuning
- **Tray**: Production container configuration with specific speed/timing profiles
- **Digital Output**: On/off control signal (coil)
- **Analog Output**: Variable voltage output (0-10V) for speed control
- **COM Port**: Serial communication port (RS-232/RS-485)
- **Production Count**: Number of items processed through the conveyor system
- **Test Mode**: Simulator mode that bypasses hardware for development/testing without Modbus devices
- **Millivolt (mV)**: Unit for analog output values (0-10000 mV = 0-10V)

---

## 14. Appendix

### 13.1 Motor Assignments
- **Motor 1-6**: Main conveyor belt sections
- **Motor 7**: [Currently unused/commented out]
- **Motor 8**: Upper Soil Belt (tray-specific calibration)

### 13.2 Default Modbus Addresses
*Defined in application constants - see source code for specific addresses*

### 13.3 State Transition Diagram
```
[Stop] ──Start──> [Run1] ──Start Timer──> [TimeDelay] ──Auto──> [Run2]
  ↑                  ↓                          ↓                  ↓
  └──────────────Stop──────────────────────────┴──────────────────┘
  
[Any State] ──E-Stop──> [E-Stop] ──Cleared──> [Buzzer Delay] ──> [Previous State]
```

---

**Document Control:**
- **Author**: Auto-generated from source code analysis
- **Review Status**: Draft
- **Next Review**: TBD
- **Change Log**: Version 1.0 - Initial document creation

