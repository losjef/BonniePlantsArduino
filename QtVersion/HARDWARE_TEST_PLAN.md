# Hardware Test Plan - ConveyorInterfaceQt
## Safety & Reliability Improvements Testing

**Branch:** feature/safety-reliability-improvements  
**Test Status:** Awaiting hardware test box construction  
**Last Updated:** 2024

---

## 1. Pre-Test Preparation

### 1.1 Backup Procedures
- [ ] Create backup of production database/counter files
- [ ] Document current UpperSoilBeltIndependant branch state
- [ ] Take photos/videos of current system operation
- [ ] Record current configuration files (JSON)

### 1.2 Test Environment Setup
- [ ] Deploy feature branch to test hardware box
- [ ] Verify Modbus connections (input/output modules)
- [ ] Confirm COM port configuration
- [ ] Validate all JSON configuration files present
- [ ] Check ProductionLog.csv is writable

### 1.3 Required Equipment
- Modbus RTU input module (digital inputs)
- Modbus RTU output module (digital + analog outputs)
- Serial cable (57600 baud, no parity, 8N1)
- Test switches for simulating sensors
- Industrial PC with touch screen
- Backup laptop for monitoring/logs

---

## 2. Critical Safety Tests

### 2.1 E-Stop Functionality
**Priority:** CRITICAL - Must pass before any other tests

#### Test 2.1.1: Normal E-Stop Operation
**Steps:**
1. Start system in Run2 state
2. Activate E-stop button
3. Verify all motors stop immediately
4. Check counter is saved to disk
5. Verify log entry created with "E-stop" marker

**Expected Results:**
- All 8 motors stop within 100ms
- Counter file updated on disk
- E-stop state entered
- Production log entry shows interruption
- No Modbus errors displayed

**Pass Criteria:** ✅ All motors stopped, data saved, no errors

---

#### Test 2.1.2: E-Stop with Modbus Failure
**Steps:**
1. Disconnect Modbus output module
2. Start system in Run2 state
3. Activate E-stop button
4. Observe error handling

**Expected Results:**
- Error dialog appears: "Critical Error: Failed to send E-stop signal to Modbus after retries. Check hardware connection!"
- System retries 3 times with 200ms delay
- Counter still saved despite Modbus failure
- Log entry marked as "E-stop (MODBUS FAILURE)"

**Pass Criteria:** ✅ Error dialog shown, retries occur, data protected

---

#### Test 2.1.3: E-Stop Recovery
**Steps:**
1. System in E-stop state (from 2.1.1)
2. Release E-stop button
3. Press Reset/Clear button
4. Attempt to start Run1

**Expected Results:**
- System clears E-stop state
- Can transition to Idle
- Can start new Run1 cycle
- Previous counter value persists

**Pass Criteria:** ✅ Normal operation resumes

---

### 2.2 Normal Stop Functionality

#### Test 2.2.1: Stop During Run2
**Steps:**
1. Start Run2 cycle
2. Wait for 50 plants counted
3. Press Stop button
4. Verify motor shutdown

**Expected Results:**
- All motors stop gracefully
- Counter saved to disk
- Production log entry created with final count
- System enters Idle state

**Pass Criteria:** ✅ Clean stop, data saved

---

### 2.3 Thread Safety Tests

#### Test 2.3.1: Concurrent State Changes
**Steps:**
1. Start Run2 cycle (scanning inputs in background)
2. Rapidly press speed change buttons (1-6)
3. Press E-stop during speed change
4. Check for crashes or race conditions

**Expected Results:**
- No application crashes
- State transitions complete atomically
- No corrupted counter values
- Mutex prevents race conditions

**Pass Criteria:** ✅ No crashes, clean state transitions

---

## 3. Motor Control Tests

### 3.1 All Motors Activation

#### Test 3.1.1: Run2 - All 8 Motors
**Purpose:** Verify fix for missing motors 5 & 6 bug

**Steps:**
1. Configure all 8 motors in motor factors
2. Select Tray 1
3. Start Run1, wait for time delay
4. Transition to Run2
5. Monitor all motor digital outputs

**Expected Results:**
- Motors 1, 2, 3, 4, 5, 6, 7, 8 all receive digital ON signal
- Previously missing motors 5 & 6 now active
- Analog speeds applied correctly to all motors

**Pass Criteria:** ✅ All 8 motors running (verify with multimeter/LED indicators)

---

### 3.2 Speed Control

#### Test 3.2.1: Six Speed Settings
**Steps:**
1. Run2 active with Tray 1
2. Press Speed 1 button → verify analog output
3. Press Speed 2 button → verify analog output
4. Repeat for speeds 3, 4, 5, 6
5. Monitor Modbus analog output values (0-100%)

**Expected Results:**
- Each speed button changes motor analog outputs
- Frame display shows correct color (Speed 1 = white, Speed 2 = blue, etc.)
- speedSelected variable updated
- No null pointer crashes (currentTray protected)

**Pass Criteria:** ✅ All 6 speeds produce correct analog outputs

---

#### Test 3.2.2: Speed Change Without Tray
**Steps:**
1. System in Idle (no tray selected)
2. Press Speed 1 button

**Expected Results:**
- Warning message: "Cannot change speed: No tray selected"
- No crash (null pointer protection works)
- speedSelected still updated for future use

**Pass Criteria:** ✅ No crash, graceful warning

---

### 3.3 Modbus Error Handling

#### Test 3.3.1: Analog Output Validation
**Steps:**
1. Manually call `writeAnalogOutput()` with invalid values:
   - Percent = -10
   - Percent = 150
   - Percent = 50 (valid)

**Expected Results:**
- -10 clamped to 0%
- 150 clamped to 100%
- 50 accepted as-is
- No crashes from out-of-range values

**Pass Criteria:** ✅ Input clamping works

---

#### Test 3.3.2: Modbus Connection Loss
**Steps:**
1. Run2 active
2. Disconnect Modbus output module mid-cycle
3. Press speed change button

**Expected Results:**
- Error dialog appears: "Modbus connection not available"
- System doesn't crash
- Can recover when reconnected

**Pass Criteria:** ✅ Graceful error handling

---

## 4. Counter & Logging Tests

### 4.1 Counter Batching

#### Test 4.1.1: Normal Counter Increment
**Steps:**
1. Start Run2 cycle
2. Monitor file writes to `counter.txt`
3. Count to 25 plants
4. Check disk write frequency

**Expected Results:**
- Counter increments in UI every plant
- Disk writes occur every 10 counts (batched)
- Writes at counts: 10, 20, 30 (not every count)
- 90% reduction in disk I/O vs old version

**Pass Criteria:** ✅ Batching works, UI still responsive

---

#### Test 4.1.2: Counter Save on Stop
**Steps:**
1. Run2 active, count to 7 plants (no batch write yet)
2. Press Stop button
3. Check `counter.txt` immediately

**Expected Results:**
- Counter file shows 7 (immediate write on Stop)
- Data not lost despite being < batch size
- Production log shows 7 plants

**Pass Criteria:** ✅ Data saved even when < 10

---

#### Test 4.1.3: Counter Save on E-Stop
**Steps:**
1. Run2 active, count to 3 plants
2. Press E-stop
3. Check `counter.txt`

**Expected Results:**
- Counter file shows 3 (immediate write on E-stop)
- Critical data protected
- Log entry shows 3 plants with "E-stop" marker

**Pass Criteria:** ✅ Emergency save works

---

### 4.2 Production Logging

#### Test 4.2.1: Automatic Log Entry Creation
**Steps:**
1. Complete full Run1 → Run2 cycle
2. Count 50 plants at Speed 3 with Tray 2
3. Press Stop
4. Open `ProductionLog.csv`

**Expected Results:**
- New entry in CSV file
- Timestamp matches stop time
- Count = 50
- Speed = 3
- Tray = Tray 2
- Total count = cumulative value

**Pass Criteria:** ✅ Log entry accurate

---

#### Test 4.2.2: Log Viewer UI
**Steps:**
1. Menu → View Production Log
2. Verify table displays entries
3. Check statistics panel
4. Click Export button
5. Click Clear button (confirm dialog)

**Expected Results:**
- Table shows all production runs
- Statistics: Total Runs, Total Plants, Avg Plants/Run
- Export creates new CSV at user location
- Clear prompts confirmation, then empties log

**Pass Criteria:** ✅ All UI functions work

---

#### Test 4.2.3: Log Persistence
**Steps:**
1. Create 5 production log entries
2. Close application
3. Reopen application
4. View Production Log

**Expected Results:**
- All 5 entries still present
- Data survives restart
- CSV file intact

**Pass Criteria:** ✅ Data persists

---

## 5. Long-Running Stability Tests

### 5.1 8-Hour Endurance Test
**Purpose:** Verify memory leaks fixed, thread safety stable

**Steps:**
1. Start Run2 cycle at Speed 4
2. Simulate continuous production for 8 hours
3. Monitor:
   - Memory usage (Task Manager)
   - Counter accuracy
   - Log file growth
   - Modbus errors

**Expected Results:**
- Memory usage stable (no leaks from QModbusReply fix)
- Counter matches actual plant count
- No crashes or hangs
- Log file grows linearly

**Pass Criteria:** ✅ 8 hours without crash, memory stable

---

### 5.2 Rapid State Changes
**Steps:**
1. Cycle through states rapidly:
   - Idle → Run1 → TimeDelay → Run2 → Stop → Idle (repeat 100x)
2. Mix in E-stops randomly
3. Change speeds during Run2

**Expected Results:**
- No crashes
- No state machine corruption
- Mutex protects concurrent access
- Clean transitions every time

**Pass Criteria:** ✅ 100 cycles without error

---

## 6. Configuration & Calibration Tests

### 6.1 Motor Factors
**Steps:**
1. Open Motor Factors dialog
2. Change factors for Motor 1
3. Save and close
4. Start Run2 at Speed 1
5. Verify speed reflects new factors

**Expected Results:**
- New factors persist to JSON
- Motor speed calculated correctly
- Changes take effect immediately

**Pass Criteria:** ✅ Calibration works

---

### 6.2 Tray Factors
**Steps:**
1. Open Tray Calibration dialog
2. Modify Tray 3 factors
3. Save
4. Select Tray 3 in main UI
5. Run at different speeds

**Expected Results:**
- Tray-specific factors applied
- Counter timer intervals adjusted
- Production counts accurate

**Pass Criteria:** ✅ Tray calibration effective

---

## 7. Rollback Testing

### 7.1 Revert to UpperSoilBeltIndependant
**Purpose:** Ensure can safely rollback if issues found

**Steps:**
1. Stop test system
2. Check out UpperSoilBeltIndependant branch
3. Rebuild application
4. Restore backed-up configuration files
5. Verify old version still works

**Expected Results:**
- Old version runs normally
- No configuration corruption
- Can revert at any time

**Pass Criteria:** ✅ Clean rollback possible

---

## 8. Test Failure Procedures

### If Critical Test Fails:
1. **Document Failure**:
   - Screenshot error dialogs
   - Copy log files
   - Note exact steps to reproduce
   
2. **Assess Safety Impact**:
   - Does E-stop work? (If no → STOP TESTING)
   - Can motors be stopped manually?
   - Is data being saved?

3. **Rollback Decision**:
   - Safety issue → immediate rollback
   - Data corruption → rollback, fix, retest
   - Minor bug → log for fix, continue testing

4. **Report to Development**:
   - GitHub issue with full details
   - Attach logs, screenshots
   - Mark as blocker if safety-related

---

## 9. Sign-Off Checklist

Before deploying to production, verify:

- [ ] All E-stop tests passed (2.1.1 - 2.1.3)
- [ ] All 8 motors activate in Run2 (3.1.1)
- [ ] Counter batching works (4.1.1 - 4.1.3)
- [ ] Production logging accurate (4.2.1 - 4.2.3)
- [ ] 8-hour stability test passed (5.1)
- [ ] Memory usage stable (no leaks)
- [ ] Thread safety confirmed (2.3.1, 5.2)
- [ ] Rollback tested successfully (7.1)
- [ ] All changes documented in SAFETY_IMPROVEMENTS_CHANGELOG.md
- [ ] Operator training completed on new log viewer

**Tested By:** _______________  
**Date:** _______________  
**Approved By:** _______________  
**Date:** _______________  

---

## 10. Known Limitations

Items NOT tested (require specific hardware):
- Actual conveyor belt speeds (need physical measurement)
- Real sensor input timing (need production load)
- Touch screen calibration on industrial PC

Items for future testing:
- Configuration UI for counter batch size (not yet implemented)
- Network logging/remote monitoring (future feature)

---

## Notes Section

Use this space for test observations, anomalies, or recommendations:

```
[Test notes go here]
```
