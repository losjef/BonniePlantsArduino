# Deployment Checklist - ConveyorInterfaceQt
## Safety & Reliability Improvements Branch

**Branch:** feature/safety-reliability-improvements  
**Target:** Merge to UpperSoilBeltIndependant (production backup branch)  
**Production System:** Industrial conveyor control - safety critical

---

## Phase 1: Pre-Deployment Preparation

### 1.1 Code Review & Testing
- [ ] All unit tests pass (if applicable)
- [ ] No compiler warnings in release build
- [ ] Code review completed and approved
- [ ] All safety improvements documented in SAFETY_IMPROVEMENTS_CHANGELOG.md
- [ ] All TODOs addressed or documented for future work

### 1.2 Hardware Test Validation
**⚠️ CRITICAL: Do not proceed without passing all hardware tests**

- [ ] E-stop functionality tested and verified (Test 2.1.1 - 2.1.3)
- [ ] All 8 motors activate in Run2 (Test 3.1.1)
- [ ] Motor speed control validated (6 speeds)
- [ ] Counter batching tested (writes every 10 counts)
- [ ] Production logging tested and verified
- [ ] 8-hour stability test passed (no crashes, memory stable)
- [ ] Thread safety confirmed (rapid state changes)
- [ ] Modbus error handling tested (connection loss scenarios)
- [ ] Rollback procedure tested successfully

**Hardware Test Status:** _______________  
**Tested By:** _______________  
**Test Date:** _______________  

### 1.3 Documentation Review
- [ ] PRD.md updated with new features
- [ ] PRODUCTION_LOGGING_DOCS.md user manual complete
- [ ] HARDWARE_TEST_PLAN.md available for reference
- [ ] README updated with deployment notes
- [ ] SAFETY_IMPROVEMENTS_CHANGELOG.md finalized

### 1.4 Configuration Backup
**⚠️ CRITICAL: Backup all configuration before deployment**

- [ ] Backup current production JSON files:
  - [ ] MotorCalibFact.json
  - [ ] TrayFactors.json
  - [ ] UpperSoilBeltFactors.json
  - [ ] COMPorts.json
  - [ ] CountDownTimer.json
- [ ] Backup counter.txt (current production count)
- [ ] Export existing ProductionLog.csv (if exists)
- [ ] Document current software version/commit hash
- [ ] Take photos/videos of current system operation

**Backup Location:** _______________  
**Backup Date:** _______________  

---

## Phase 2: Deployment Environment Setup

### 2.1 System Preparation
- [ ] Verify industrial PC meets requirements (Windows 10/11, 64-bit)
- [ ] Qt 6.6.3 runtime libraries installed
- [ ] MinGW 64-bit runtime available (if not statically linked)
- [ ] Visual C++ Redistributable installed (if needed)
- [ ] Sufficient disk space (min 500MB for logs/backups)

### 2.2 Communication Hardware
- [ ] Modbus input module connected (COM port: _______)
- [ ] Modbus output module connected (COM port: _______)
- [ ] Serial cables tested (57600 baud, 8N1)
- [ ] COM port numbers documented
- [ ] Modbus device addresses confirmed:
  - Digital input device: 1
  - Digital output device: 1
  - Analog output device: (verify)

### 2.3 Operator Notification
**⚠️ Inform production staff before deployment**

- [ ] Schedule downtime window (recommended: 30-60 minutes)
- [ ] Notify operators of new features:
  - Production logging with export capability
  - Improved E-stop safety (retry logic)
  - Counter optimization (batched writes)
- [ ] Train operators on "View Production Log" menu
- [ ] Distribute PRODUCTION_LOGGING_DOCS.md user guide
- [ ] Provide rollback contact information

**Downtime Window:** _______________  
**Operators Notified:** _______________  

---

## Phase 3: Staged Deployment

### 3.1 Build Release Version
- [ ] Set build configuration to Release (not Debug)
- [ ] Clean build directory: `cmake --build build --target clean`
- [ ] Rebuild: `cmake --build build --config Release`
- [ ] Verify executable size/timestamp
- [ ] Test executable on development machine first

**Build Command:**
```bash
cd c:\Users\jeffl\source\repos\Github\BonniePlant02\ConveyorInterfaceQt
cmake --build build --config Release
```

### 3.2 Stop Production System
**⚠️ Follow safe shutdown procedure**

- [ ] Complete any in-progress production runs
- [ ] Press Stop button (not E-stop)
- [ ] Verify all motors have stopped
- [ ] Verify counter saved to disk (check counter.txt timestamp)
- [ ] Exit current application cleanly
- [ ] Wait 10 seconds for Modbus connections to close

### 3.3 Deploy New Executable
- [ ] Copy new executable to production PC:
  - Source: `build\Release\ConveyorInterfaceQt.exe`
  - Destination: (production install path)
- [ ] Rename old executable to `ConveyorInterfaceQt_backup_YYYYMMDD.exe`
- [ ] Copy new executable to production folder
- [ ] Verify file permissions (read/execute for operators)

### 3.4 Restore Configuration Files
**⚠️ CRITICAL: Do not overwrite existing calibration data**

- [ ] Verify JSON files still present in production folder
- [ ] If missing, restore from Phase 1.4 backup
- [ ] Do NOT copy development JSON files (they have test values)
- [ ] Verify ProductionLog.csv exists (create empty if needed)

### 3.5 Initial Startup
- [ ] Launch new ConveyorInterfaceQt executable
- [ ] Verify splash screen/version info (if available)
- [ ] Check terminal/log output for errors
- [ ] Verify Modbus connections established:
  - Status bar should show "Connected" or similar
  - No critical errors in console
- [ ] Confirm all configuration loaded:
  - Motor factors displayed correctly
  - Tray selections available
  - Wait time shows previous value

**Startup Status:** _______________  
**Connection Status:** _______________  

---

## Phase 4: Post-Deployment Validation

### 4.1 Functional Verification (10-15 minutes)

#### Test 4.1.1: E-Stop Safety Test
- [ ] Press E-stop button
- [ ] Verify all motors stop immediately
- [ ] Verify red light activates
- [ ] Check counter saved (counter.txt updated)
- [ ] Release E-stop and clear
- [ ] Verify system can resume normal operation

**E-Stop Test:** PASS / FAIL  

#### Test 4.1.2: Motor Control Test
- [ ] Select Tray 1
- [ ] Start Run1
- [ ] Wait for TimeDelay countdown
- [ ] Verify transition to Run2
- [ ] **CRITICAL:** Verify ALL 8 motors running (visual/multimeter)
  - Motor 1: ______
  - Motor 2: ______
  - Motor 3: ______
  - Motor 4: ______
  - Motor 5: ______ (was missing - now fixed)
  - Motor 6: ______ (was missing - now fixed)
  - Motor 7: ______
  - Motor 8: ______

**Motor Test:** PASS / FAIL  

#### Test 4.1.3: Speed Change Test
- [ ] During Run2, press Speed 1 button
- [ ] Verify frame color changes (white)
- [ ] Press Speed 2, 3, 4, 5, 6 buttons
- [ ] Verify no crashes or errors
- [ ] Confirm motor speeds adjust (observe belt speed if possible)

**Speed Test:** PASS / FAIL  

#### Test 4.1.4: Counter & Logging Test
- [ ] Start new production run
- [ ] Let counter reach at least 25 plants
- [ ] Press Stop button
- [ ] Verify production log entry created
- [ ] Menu → View Production Log
- [ ] Verify entry shows correct count, speed, tray
- [ ] Test Export button (save to USB/network location)

**Logging Test:** PASS / FAIL  

### 4.2 Monitoring (First 2 Hours)
**Operator should monitor system closely**

- [ ] Monitor for unexpected stops
- [ ] Watch for Modbus communication errors
- [ ] Verify counter increments correctly
- [ ] Check ProductionLog.csv file size (should grow slowly)
- [ ] Monitor PC CPU usage (should be < 20%)
- [ ] Monitor memory usage (should be stable ~50-100MB)

**Monitoring Status:** _______________  
**Issues Observed:** _______________  

### 4.3 First Production Run
- [ ] Complete one full production batch
- [ ] Verify counter accuracy (manual count vs system count)
- [ ] Check production log data accuracy
- [ ] Verify counter saved properly on Stop
- [ ] Test speed changes during production
- [ ] Verify all motors maintain speeds

**Production Run Status:** _______________  
**Count Accuracy:** _______________  

---

## Phase 5: Rollback Procedure (If Needed)

**⚠️ Execute immediately if critical issues found**

### 5.1 Identify Critical Issues
Rollback immediately if:
- E-stop does not stop motors
- Crashes/freezes during production
- Counter data corrupted/lost
- Modbus communication failures
- Any safety-related malfunction

### 5.2 Emergency Rollback Steps
1. [ ] Press Stop button (complete current run if safe)
2. [ ] Exit application
3. [ ] Rename new executable: `ConveyorInterfaceQt_FAILED_YYYYMMDD.exe`
4. [ ] Rename backup executable back to: `ConveyorInterfaceQt.exe`
5. [ ] Restore configuration files from Phase 1.4 backup
6. [ ] Restart old version
7. [ ] Verify old version works normally
8. [ ] Document failure details for development team

### 5.3 Report Rollback
- [ ] GitHub issue created with details
- [ ] Screenshots/logs attached
- [ ] Contact development team: _______________
- [ ] Schedule re-deployment after fixes

**Rollback Reason:** _______________  
**Rollback Time:** _______________  

---

## Phase 6: Sign-Off & Documentation

### 6.1 Deployment Success Criteria
- [ ] All Phase 4 tests passed (E-stop, motors, speed, counter, logging)
- [ ] No critical errors in first 2 hours
- [ ] First production run completed successfully
- [ ] Operators trained on new features
- [ ] Production logging verified working
- [ ] Performance stable (no memory leaks, CPU normal)

### 6.2 Final Documentation
- [ ] Update production system documentation:
  - Software version: _______________
  - Git commit hash: _______________
  - Deployment date: _______________
- [ ] Update configuration backup schedule
- [ ] Archive deployment logs/screenshots
- [ ] Update operator training materials

### 6.3 Git Repository Update
- [ ] Merge feature branch to UpperSoilBeltIndependant:
  ```bash
  git checkout UpperSoilBeltIndependant
  git merge feature/safety-reliability-improvements
  git push origin UpperSoilBeltIndependant
  ```
- [ ] Tag release:
  ```bash
  git tag -a v1.1-safety-improvements -m "Safety & reliability improvements"
  git push origin v1.1-safety-improvements
  ```
- [ ] Update GitHub release notes

### 6.4 Sign-Off
**All stakeholders must approve deployment**

- [ ] Operations Manager: _______________  Date: _______________
- [ ] Safety Officer: _______________  Date: _______________
- [ ] Lead Operator: _______________  Date: _______________
- [ ] Developer/Engineer: _______________  Date: _______________

---

## Post-Deployment Monitoring Schedule

### Week 1: Daily Checks
- [ ] Day 1: Monitor continuously
- [ ] Day 2: Check logs, memory usage
- [ ] Day 3: Verify production accuracy
- [ ] Day 4: Review ProductionLog.csv data
- [ ] Day 5: Operator feedback session
- [ ] Day 6-7: Continue monitoring

### Week 2-4: Weekly Checks
- [ ] Week 2: Production log analysis, performance review
- [ ] Week 3: Long-term stability check (memory leaks?)
- [ ] Week 4: Final deployment review meeting

**Monitoring Sign-Off:** _______________  Date: _______________  

---

## Additional Notes / Observations

```
[Document any issues, observations, or recommendations here]




```

---

## Emergency Contacts

**Development Team:**  
Name: _______________  
Phone: _______________  
Email: _______________  

**System Administrator:**  
Name: _______________  
Phone: _______________  

**Production Manager:**  
Name: _______________  
Phone: _______________  

**After-Hours Emergency:**  
Phone: _______________  
