# Test Mode / Simulator Documentation

## Overview

The ConveyorInterfaceQt application includes a **Test Mode** (simulator) that allows full functionality testing without physical hardware. This is useful for:
- Software development and debugging
- UI/UX testing
- State machine validation
- Production logging verification
- Training and demonstrations
- Testing before hardware test box is ready

## Enabling Test Mode

Test mode is activated via an environment variable:

### Windows (Command Line)
```cmd
set CONVEYOR_TEST_MODE=1
ConveyorInterfaceQt.exe
```

### Windows (PowerShell)
```powershell
$env:CONVEYOR_TEST_MODE=1
.\ConveyorInterfaceQt.exe
```

### Windows (Batch File)
Use the provided `LaunchTestMode.bat`:
```cmd
LaunchTestMode.bat
```

### Linux/macOS
```bash
export CONVEYOR_TEST_MODE=1
./ConveyorInterfaceQt
```

## Test Mode Features

### Visual Indicators

When test mode is active, you'll see:

1. **Window Title**: Shows `[TEST MODE]` suffix
   ```
   Bonnie Plant Conveyor Interface [TEST MODE]
   ```

2. **Status Bar**: Yellow background with warning icon
   ```
   ⚠️ TEST MODE - Simulated Hardware
   ```

### Simulated Hardware

#### Modbus Communication
- All Modbus write operations return immediate success
- No actual serial port communication occurs
- Digital outputs (motor on/off) are logged but not sent
- Analog outputs (motor speeds) are logged but not sent
- Connection errors are bypassed

#### Input Simulation
Test mode provides UI buttons for simulating hardware inputs:

- **Simulate RUN1 Button** (Green)
  - Triggers Run1 state transition
  - Same as pressing physical Run1 button
  
- **Simulate STOP Button** (Pink)
  - Triggers Stop state
  - Same as pressing physical Stop button
  
- **Simulate E-STOP (Emergency)** (Red)
  - Triggers emergency stop state
  - First click: activates E-stop
  - Second click: clears E-stop
  - Tests critical safety logic

### Normal Functionality

All other features work exactly as in production mode:

✅ **State Machine**
- All state transitions (Stop → Run1 → TimeDelay → Run2 → Stop)
- E-stop handling and recovery
- Buzzer delay states

✅ **Timer-Based Counting**
- Normal real-time counting (same speeds as production)
- Counter increments based on tray factors
- Batched disk writes (every 10 counts)

✅ **Configuration Dialogs**
- Motor Factors
- Tray Calibration Factors
- Upper Soil Belt Factors
- COM Ports (displayed but not used)

✅ **Production Logging**
- Full logging functionality
- ProductionLog.csv created and updated
- Log viewer works normally
- Export and clear functions active

✅ **Speed Controls**
- All 6 speed buttons functional
- Speed calculations work normally
- Tray selection active

✅ **UI Elements**
- All displays update correctly
- Counters increment normally
- Timers count down
- Visual feedback works

## Testing Workflow

### Basic Operation Test
1. Launch in test mode: `LaunchTestMode.bat`
2. Verify window shows `[TEST MODE]`
3. Select a tray (e.g., Tray 1)
4. Select a speed (e.g., Speed 3)
5. Click "Simulate RUN1 Button"
6. Wait for TimeDelay countdown
7. Observe transition to Run2
8. Watch counter increment (timer-based)
9. Click "Simulate STOP Button"
10. Verify production log entry created

### E-Stop Safety Test
1. Start Run2 (follow Basic Operation steps 1-8)
2. Let counter reach ~20 plants
3. Click "Simulate E-STOP (Emergency)" button
4. Verify:
   - Counter saved immediately
   - State changes to E-Stop
   - Red indicator (if visible)
5. Click "Simulate E-STOP" again to clear
6. System should return to previous state

### Speed Change Test
1. Run2 active
2. Cycle through all 6 speeds
3. Verify frame display colors change
4. Counter rate should adjust (based on tray factors)

### Production Logging Test
1. Complete 3 full production runs with different:
   - Tray types
   - Speeds
   - Count durations
2. Menu → View Production Log
3. Verify all 3 entries logged correctly
4. Test Export function
5. Verify CSV file readable

## Console Output

Test mode produces detailed logging:

```
***** TEST MODE ENABLED *****
Modbus communication will be simulated
Test mode: Modbus client mocked (no hardware required)
Test mode UI controls added

[TEST MODE] Digital Write simulated - Address: 0 Value: 1
[TEST MODE] Analog Write simulated - Motor: 0 Speed: 75%
[TEST MODE] Simulating RUN1 button press
[TEST MODE] Simulating E-STOP activation
```

Monitor the console to verify operations are simulated correctly.

## Limitations

### Not Simulated
- Physical motor movement (obvious)
- Real sensor inputs (must use UI buttons)
- Actual conveyor timing (uses calculated times)
- Hardware failures/errors
- Serial port scanning (CreateInputScanThread skipped)

### Same as Production
- Counter persistence (writes to disk)
- Configuration file I/O (JSON files)
- Production logging (CSV file)
- Timer accuracy
- State machine logic
- Memory management
- Thread safety

## Switching to Production Mode

To disable test mode and run normally:

1. **Launch normally** (without environment variable):
   ```cmd
   ConveyorInterfaceQt.exe
   ```

2. **Verify production mode**:
   - No `[TEST MODE]` in window title
   - No yellow status bar
   - No simulation buttons visible
   - Modbus connection required

3. **Ensure hardware connected**:
   - COM ports configured correctly
   - Modbus modules powered and connected
   - Serial cables intact

## Troubleshooting

### "Test mode enabled but Modbus errors still appear"
- Check that environment variable is set **before** launching
- Restart application after setting variable
- Verify `CONVEYOR_TEST_MODE=1` exactly (case-sensitive on Linux)

### "Simulation buttons not visible"
- Verify status bar shows "TEST MODE"
- Check window title for `[TEST MODE]` suffix
- Look for yellow test panel (may need to scroll/resize window)

### "Counter not incrementing"
- Timer-based counting still requires:
  - Tray selected
  - Speed selected
  - Run2 state active
- Check console for timer start messages

### "Production log not working"
- Test mode uses same logging as production
- Check file permissions on working directory
- Verify ProductionLog.csv is writable

## Development Notes

### Code Locations

Test mode implementation:
- **Detection**: `mainwindow.cpp` constructor, checks `CONVEYOR_TEST_MODE` env var
- **Modbus mocking**: `createQModbusRtuSerialClient()` returns early
- **Digital output**: `writedigitalout.cpp` bypasses Modbus calls
- **Analog output**: `writeanalogoutput.cpp` bypasses Modbus calls
- **UI setup**: `setupTestModeUI()` creates simulation buttons
- **Simulation handlers**: `simulateRun1Button()`, `simulateStopButton()`, `simulateEStop()`

### Adding More Simulations

To add more simulated inputs (e.g., Start Delay button):

1. Add button in `setupTestModeUI()`:
```cpp
QPushButton* btnSimDelay = new QPushButton("Simulate START DELAY", testPanel);
connect(btnSimDelay, &QPushButton::clicked, this, &MainWindow::simulateStartDelayButton);
layout->addWidget(btnSimDelay);
```

2. Add handler in `mainwindow.cpp`:
```cpp
void MainWindow::simulateStartDelayButton()
{
    qInfo() << "[TEST MODE] Simulating START DELAY button";
    on_pushButtonStartTimer_clicked();
}
```

3. Declare in `mainwindow.h`:
```cpp
void simulateStartDelayButton();
```

## Best Practices

### Development
- ✅ Always develop with test mode first
- ✅ Test all state transitions before hardware
- ✅ Verify production logging in test mode
- ✅ Use test mode for UI/UX iterations

### Testing
- ✅ Test complete workflows (Run1 → Run2 → Stop)
- ✅ Test error paths (E-stop during different states)
- ✅ Verify counter persistence across restarts
- ✅ Compare test mode behavior to production expectations

### Deployment
- ❌ Never deploy with `CONVEYOR_TEST_MODE` set in production
- ✅ Verify test mode is **disabled** on production systems
- ✅ Use test mode for training operators safely
- ✅ Document any test mode differences for stakeholders

## FAQ

**Q: Does test mode affect production data?**  
A: Yes - counter.txt and ProductionLog.csv are real files. Test mode writes actual data to disk.

**Q: Can I test hardware failures?**  
A: No - test mode always simulates success. Use real hardware for failure testing.

**Q: Will test mode work on the production PC?**  
A: Yes, but be careful not to confuse test data with real production data.

**Q: How do I know the test is accurate?**  
A: Test mode uses the same code paths as production, only Modbus I/O is mocked. State machine, timers, and logic are identical.

**Q: Can I use test mode for operator training?**  
A: Yes! Test mode is perfect for training without risk to hardware.

---

**For questions or issues with test mode, contact the development team.**
