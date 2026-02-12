/**
 * @file main.cpp
 * @brief BonnieConveyor — P1AM Arduino Controller
 *
 * Headless conveyor-line controller running on a P1AM-100 (SAMD21) CPU
 * inside a P1-01AC base.  All operator interaction is through the
 * CM5-T15W C-more HMI, which connects as a Modbus TCP client.
 *
 * I/O is accessed directly through the P1AM library (SPI backplane),
 * NOT through Modbus to the I/O modules.
 *
 * State machine:
 *   STOP → (start) → BUZZER_DELAY → RUN1 → (delay btn) → TIME_DELAY
 *        → (timer done) → RUN2 → (delay btn) → TIME_DELAY → …
 *   Any state → (estop) → ESTOP → (clear) → BUZZER_DELAY → prior state
 *   Any state (except ESTOP) → (stop) → STOP
 */

// ── Includes ────────────────────────────────────────────────────────────
#include <Arduino.h>
#include <SPI.h>
#include <P1AM.h>
#include <Ethernet.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <FlashStorage.h>

#include "Config.h"

// ── Flash Storage ───────────────────────────────────────────────────────
FlashStorage(flashCalib,   CalibrationData);
FlashStorage(flashCounter, CounterData);

// ── Ethernet / Modbus objects ───────────────────────────────────────────
byte           mac[]  = { DEFAULT_MAC[0], DEFAULT_MAC[1], DEFAULT_MAC[2],
                          DEFAULT_MAC[3], DEFAULT_MAC[4], DEFAULT_MAC[5] };
IPAddress      ip(DEFAULT_IP[0], DEFAULT_IP[1], DEFAULT_IP[2], DEFAULT_IP[3]);
IPAddress      subnet(DEFAULT_SUBNET[0], DEFAULT_SUBNET[1],
                      DEFAULT_SUBNET[2], DEFAULT_SUBNET[3]);
IPAddress      gateway(DEFAULT_GATEWAY[0], DEFAULT_GATEWAY[1],
                       DEFAULT_GATEWAY[2], DEFAULT_GATEWAY[3]);

EthernetServer ethServer(MODBUS_TCP_PORT);
ModbusTCPServer modbusTCP;
EthernetClient  modbusClient;

// ── Calibration & Counter ───────────────────────────────────────────────
CalibrationData calib;
uint32_t        totalCounter         = 0;
uint32_t        currentCounter       = 0;
uint32_t        countersSinceFlush   = 0;

// ── State Machine ───────────────────────────────────────────────────────
SystemState     currentState         = STATE_STOP;
SystemState     previousState        = STATE_STOP;
SystemState     estopReturnState     = STATE_STOP;

int             speedSelected        = 0;   // 1-6, 0 = none
int             traySelected         = 0;   // 1-6, 0 = none
float           motorSpeed[NUM_MOTORS] = {};
uint16_t        outputState          = 0;   // P1-16TR bitmask
uint16_t        prevInputs           = 0xFFFF; // NC default high

// ── Timers (millis-based) ───────────────────────────────────────────────
int             waitTime;                  // from calibration
int             remainingTime        = 0;
unsigned long   lastCountdownTick    = 0;
unsigned long   lastCounterTick      = 0;
unsigned long   counterIntervalMs    = 2000;

unsigned long   buzzerStartMs        = 0;
bool            buzzerSounding       = false;

unsigned long   lastHeartbeatMs      = 0;
bool            heartbeatToggle      = false;

unsigned long   lastScanMs           = 0;

// Previous HMI selections (for edge detection)
int             prevHMISpeed         = 0;
int             prevHMITray          = 0;

// ── Forward Declarations ────────────────────────────────────────────────
// State machine
void stateStop();
void stateRun1();
void stateTimeDelay();
void stateRun2();
void stateEstop();
void stateBuzzerDelay();
void stateTimeDelayComplete();
void stateEstopCleared();

// I/O
void scanInputs();
void updateOutputs();
void setMotorSpeeds();

// Helpers
void setOutputBit(uint8_t bit, bool on);
void setRedLight();
void setYellowLight();
void setGreenLight();
void setBuzzer(bool on);
void allMotorsOn();
void allMotorsOff();
void partialMotorsOff();

// Timers
void handleCountdownTimer();
void handleCounterTimer();
void handleBuzzerTimer();
void handleHeartbeat();

// Modbus / HMI
void handleModbus();
void processHMICommands();
void updateStatusRegisters();
void pushCalibrationToRegisters();
void pullCalibrationFromRegisters();

// Persistence
void loadCalibration();
void saveCalibration();
void loadCounter();
void saveCounter();

// Logging
void logProductionRun();

// =====================================================================
//  SETUP
// =====================================================================
void setup() {
    Serial.begin(115200);
    unsigned long t0 = millis();
    while (!Serial && (millis() - t0 < 3000));  // wait for serial, max 3 s

    Serial.println(F("=== BonnieConveyor P1AM v1.0 ==="));
    Serial.println(F("Waiting for P1AM base..."));

    while (!P1.init()) {
        delay(100);
    }
    Serial.println(F("P1AM base initialised"));

    // Configure watchdog: 5 s timeout, outputs go off on trip
    P1.configWD(5000, TOGGLE);

    // ── Ethernet ────────────────────────────────────────────────────
    Ethernet.init(ETH_CS_PIN);
    Ethernet.begin(mac, ip, gateway, gateway, subnet);
    ethServer.begin();
    Serial.print(F("Modbus TCP server at "));
    Serial.print(Ethernet.localIP());
    Serial.print(F(":"));
    Serial.println(MODBUS_TCP_PORT);

    // ── Modbus TCP Server ───────────────────────────────────────────
    if (!modbusTCP.begin()) {
        Serial.println(F("Modbus TCP server FAILED"));
        while (1) delay(1000);
    }
    modbusTCP.configureHoldingRegisters(0, Reg::TOTAL_REGISTERS);
    modbusTCP.configureDiscreteInputs(0, 16);

    // ── Load Calibration & Counter ──────────────────────────────────
    loadCalibration();
    loadCounter();
    waitTime      = calib.waitTime;
    remainingTime = waitTime;

    // Push factory/saved calibration into Modbus registers
    pushCalibrationToRegisters();

    // ── Initial safe state ──────────────────────────────────────────
    outputState = 0;
    updateOutputs();
    for (int i = 0; i < NUM_MOTORS; i++) {
        P1.writeAnalog(SLOT_AO, MOTOR_DEFS[i].analogCh, 0);
    }

    stateStop();
    Serial.println(F("=== Ready ==="));
}

// =====================================================================
//  LOOP  —  deterministic scan cycle
// =====================================================================
void loop() {
    P1.petWD();                         // reset watchdog

    handleModbus();                     // accept HMI connections, poll
    scanInputs();                       // read P1-16ND3, edge detect
    processHMICommands();               // read HMI command registers

    handleCountdownTimer();             // TimeDelay countdown
    handleCounterTimer();               // plant counter
    handleBuzzerTimer();                // buzzer pre-start delay
    handleHeartbeat();                  // heartbeat register toggle

    updateOutputs();                    // write P1-16TR + P1-08DAL-2
    updateStatusRegisters();            // push status → Modbus registers

    // Enforce minimum scan cycle for consistent timing
    unsigned long elapsed = millis() - lastScanMs;
    if (elapsed < SCAN_CYCLE_MS) {
        delay(SCAN_CYCLE_MS - elapsed);
    }
    lastScanMs = millis();
}

// =====================================================================
//  STATE MACHINE
// =====================================================================

void stateStop() {
    previousState = currentState;
    currentState  = STATE_STOP;
    Serial.println(F("State: STOP"));

    if (countersSinceFlush > 0) { saveCounter(); countersSinceFlush = 0; }

    allMotorsOff();
    setRedLight();

    remainingTime = waitTime;
}

void stateRun1() {
    previousState = currentState;
    currentState  = STATE_RUN1;
    Serial.println(F("State: RUN1"));

    setMotorSpeeds();
    allMotorsOn();
    setGreenLight();

    // start counter timer
    lastCounterTick = millis();
}

void stateTimeDelay() {
    previousState = currentState;
    currentState  = STATE_TIME_DELAY;
    Serial.println(F("State: TIME_DELAY"));

    partialMotorsOff();   // motors 1, 2, 8 off
    setYellowLight();

    // start countdown
    remainingTime     = waitTime;
    lastCountdownTick = millis();
}

void stateRun2() {
    previousState = currentState;
    currentState  = STATE_RUN2;
    Serial.println(F("State: RUN2"));

    setMotorSpeeds();
    allMotorsOn();
    setGreenLight();

    lastCounterTick = millis();
}

void stateEstop() {
    estopReturnState = previousState;
    previousState    = currentState;
    currentState     = STATE_ESTOP;
    Serial.println(F("*** E-STOP ***"));

    if (countersSinceFlush > 0) { saveCounter(); countersSinceFlush = 0; }

    allMotorsOff();
    setRedLight();
    setOutputBit(BIT_ESTOP_OUT, true);
}

void stateBuzzerDelay() {
    previousState = currentState;
    currentState  = STATE_BUZZER_DELAY;
    Serial.println(F("State: BUZZER_DELAY"));

    buzzerStartMs  = millis();
    buzzerSounding = true;
    setBuzzer(true);
}

void stateTimeDelayComplete() {
    logProductionRun();
    currentCounter = 0;
    Serial.println(F("TimeDelay done — counter reset"));
    stateRun2();
}

void stateEstopCleared() {
    Serial.println(F("E-Stop cleared"));
    setOutputBit(BIT_ESTOP_OUT, false);

    if (previousState == STATE_STOP) {
        stateStop();
    } else if (previousState == STATE_TIME_DELAY) {
        stateTimeDelay();
    } else {
        // Was running — give buzzer warning before resuming
        estopReturnState = previousState;  // RUN1 or RUN2
        stateBuzzerDelay();
    }
}

// =====================================================================
//  OUTPUT HELPERS
// =====================================================================

void setOutputBit(uint8_t bit, bool on) {
    if (on) outputState |=  (1u << bit);
    else    outputState &= ~(1u << bit);
}

void allMotorsOn() {
    outputState |= MOTOR_ALL_MASK;
}

void allMotorsOff() {
    outputState &= ~MOTOR_ALL_MASK;
    for (int i = 0; i < NUM_MOTORS; i++)
        motorSpeed[i] = 0.0f;
}

void partialMotorsOff() {
    // Stop motors 1, 2, 8 during TimeDelay
    outputState &= ~MOTOR_PARTIAL_MASK;
}

void setRedLight() {
    setOutputBit(BIT_RED_LIGHT, true);
    setOutputBit(BIT_YEL_LIGHT, false);
    setOutputBit(BIT_GRN_LIGHT, false);
}

void setYellowLight() {
    setOutputBit(BIT_RED_LIGHT, false);
    setOutputBit(BIT_YEL_LIGHT, true);
    setOutputBit(BIT_GRN_LIGHT, false);
}

void setGreenLight() {
    setOutputBit(BIT_RED_LIGHT, false);
    setOutputBit(BIT_YEL_LIGHT, false);
    setOutputBit(BIT_GRN_LIGHT, true);
}

void setBuzzer(bool on) {
    setOutputBit(BIT_BUZZER, on);
}

// =====================================================================
//  MOTOR SPEED CALCULATION
// =====================================================================

void setMotorSpeeds() {
    if (speedSelected < 1 || speedSelected > NUM_SPEEDS) return;
    if (traySelected  < 1 || traySelected  > NUM_TRAYS)  return;

    int si = speedSelected - 1;   // 0-based speed index
    int ti = traySelected  - 1;   // 0-based tray  index

    // Motors 1-6
    for (int m = 0; m < NUM_MOTORS - 1; m++) {
        motorSpeed[m] = BASE_SPEED_PCT * calib.motorFactors[m][si];
    }
    // Motor 8 uses tray-specific factor
    motorSpeed[MOTOR_8_IDX] = BASE_SPEED_PCT * calib.trayMotor8Factors[ti][si];

    // Update counter interval for this speed/tray combination
    counterIntervalMs = static_cast<unsigned long>(
        calib.trayTimeFactors[ti][si] * 1000.0f);
    if (counterIntervalMs < 100) counterIntervalMs = 100;  // floor
}

// =====================================================================
//  PHYSICAL I/O
// =====================================================================

void updateOutputs() {
    // Write all 16 relay outputs atomically
    P1.writeDiscrete(SLOT_DO, outputState);

    // Write analog speed values for each motor
    for (int i = 0; i < NUM_MOTORS; i++) {
        // Only write non-zero if the relay is on
        bool relayOn = outputState & (1u << MOTOR_DEFS[i].relayBit);
        int  dacVal  = relayOn ? percentToDAC(motorSpeed[i]) : 0;
        P1.writeAnalog(SLOT_AO, MOTOR_DEFS[i].analogCh, dacVal);
    }
}

void scanInputs() {
    uint16_t raw = static_cast<uint16_t>(P1.readDiscrete(SLOT_DI));

    // ── START button (NO — rising edge) ─────────────────────────────
    if ((raw & (1u << BIT_START)) && !(prevInputs & (1u << BIT_START))) {
        if (currentState == STATE_STOP) {
            if (speedSelected >= 1 && traySelected >= 1) {
                stateBuzzerDelay();
            } else {
                Serial.println(F("Start ignored: select speed & tray first"));
            }
        }
    }

    // ── STOP button (NC — falling edge = activated) ─────────────────
    if (!(raw & (1u << BIT_STOP)) && (prevInputs & (1u << BIT_STOP))) {
        if (currentState != STATE_ESTOP) {
            stateStop();
        }
    }

    // ── START DELAY button (NO — rising edge) ───────────────────────
    if ((raw & (1u << BIT_START_DELAY)) && !(prevInputs & (1u << BIT_START_DELAY))) {
        if (currentState == STATE_RUN1 || currentState == STATE_RUN2) {
            stateTimeDelay();
        }
    }

    // ── E-STOP (NC — falling edge = activated) ──────────────────────
    if (!(raw & (1u << BIT_ESTOP)) && (prevInputs & (1u << BIT_ESTOP))) {
        stateEstop();
    }

    // ── E-STOP released (NC — rising edge = cleared) ────────────────
    if ((raw & (1u << BIT_ESTOP)) && !(prevInputs & (1u << BIT_ESTOP))) {
        if (currentState == STATE_ESTOP) {
            stateEstopCleared();
        }
    }

    prevInputs = raw;

    // Mirror inputs into Modbus discrete-input registers for HMI
    for (int i = 0; i < 16; i++) {
        modbusTCP.discreteInputWrite(i, (raw >> i) & 1);
    }
}

// =====================================================================
//  TIMER HANDLING
// =====================================================================

void handleCountdownTimer() {
    if (currentState != STATE_TIME_DELAY) return;

    if (millis() - lastCountdownTick >= 1000) {
        lastCountdownTick = millis();
        remainingTime--;

        if (remainingTime == BUZZER_PRE_SEC) {
            setBuzzer(true);
        } else if (remainingTime <= 1) {
            setBuzzer(false);
        }

        if (remainingTime <= 0) {
            remainingTime = 0;
            stateTimeDelayComplete();
        }
    }
}

void handleCounterTimer() {
    if (currentState != STATE_RUN1 && currentState != STATE_RUN2) return;

    if (millis() - lastCounterTick >= counterIntervalMs) {
        lastCounterTick = millis();

        currentCounter++;
        totalCounter++;
        countersSinceFlush++;

        if (countersSinceFlush >= COUNTER_BATCH_SIZE) {
            saveCounter();
            countersSinceFlush = 0;
        }
    }
}

void handleBuzzerTimer() {
    if (currentState != STATE_BUZZER_DELAY) return;

    unsigned long elapsed = (millis() - buzzerStartMs) / 1000;

    if (elapsed >= (unsigned long)(BUZZER_PRE_SEC - 1) && buzzerSounding) {
        setBuzzer(false);
        buzzerSounding = false;
    }

    if (elapsed >= (unsigned long)BUZZER_PRE_SEC) {
        // Transition to the appropriate run state
        if (previousState == STATE_STOP) {
            stateRun1();
        } else if (previousState == STATE_ESTOP) {
            if (estopReturnState == STATE_RUN1)      stateRun1();
            else if (estopReturnState == STATE_RUN2) stateRun2();
            else                                     stateStop();
        } else {
            stateRun1();  // fallback
        }
    }
}

void handleHeartbeat() {
    if (millis() - lastHeartbeatMs >= HEARTBEAT_MS) {
        lastHeartbeatMs = millis();
        heartbeatToggle = !heartbeatToggle;
    }
}

// =====================================================================
//  MODBUS TCP SERVER  (for CM5-T15W HMI)
// =====================================================================

void handleModbus() {
    // Accept new HMI connection (one client at a time)
    EthernetClient newClient = ethServer.available();
    if (newClient) {
        if (modbusClient && modbusClient.connected()) {
            newClient.stop();   // already have a client
        } else {
            modbusClient = newClient;
            modbusTCP.accept(modbusClient);
            Serial.println(F("HMI connected"));
        }
    }

    // Poll for Modbus requests (non-blocking)
    if (modbusClient && modbusClient.connected()) {
        modbusTCP.poll();
    }
}

void processHMICommands() {
    // ── One-shot command register ───────────────────────────────────
    int cmd = (int)modbusTCP.holdingRegisterRead(Reg::COMMAND);
    if (cmd != Cmd::NONE) {
        modbusTCP.holdingRegisterWrite(Reg::COMMAND, 0);  // clear immediately

        switch (cmd) {
        case Cmd::START:
            if (currentState == STATE_STOP && speedSelected >= 1 && traySelected >= 1)
                stateBuzzerDelay();
            break;

        case Cmd::STOP:
            if (currentState != STATE_ESTOP)
                stateStop();
            break;

        case Cmd::START_DELAY:
            if (currentState == STATE_RUN1 || currentState == STATE_RUN2)
                stateTimeDelay();
            break;

        case Cmd::ESTOP:
            stateEstop();
            break;

        case Cmd::ESTOP_CLEAR:
            if (currentState == STATE_ESTOP)
                stateEstopCleared();
            break;

        case Cmd::RESET_CURRENT:
            logProductionRun();
            currentCounter = 0;
            break;
        }
    }

    // ── Persistent speed selection ──────────────────────────────────
    int sp = (int)modbusTCP.holdingRegisterRead(Reg::SPEED_SELECT);
    if (sp != prevHMISpeed && sp >= 1 && sp <= NUM_SPEEDS) {
        prevHMISpeed  = sp;
        speedSelected = sp;
        Serial.print(F("Speed → ")); Serial.println(sp);
        if (traySelected >= 1) setMotorSpeeds();
    }

    // ── Persistent tray selection ───────────────────────────────────
    int tr = (int)modbusTCP.holdingRegisterRead(Reg::TRAY_SELECT);
    if (tr != prevHMITray && tr >= 1 && tr <= NUM_TRAYS) {
        prevHMITray  = tr;
        traySelected = tr;
        Serial.print(F("Tray → ")); Serial.println(TRAY_NAMES[tr - 1]);
        if (speedSelected >= 1) setMotorSpeeds();
    }

    // ── One-shot timer adjustment ───────────────────────────────────
    int adj = (int)(int16_t)modbusTCP.holdingRegisterRead(Reg::TIMER_ADJUST);
    if (adj != 0) {
        modbusTCP.holdingRegisterWrite(Reg::TIMER_ADJUST, 0);
        waitTime += adj;
        if (waitTime < 0) waitTime = 0;
        remainingTime = waitTime;
        Serial.print(F("Timer adjust ")); Serial.print(adj);
        Serial.print(F(" → ")); Serial.println(waitTime);
    }

    // ── Save timer ──────────────────────────────────────────────────
    if (modbusTCP.holdingRegisterRead(Reg::SAVE_TIMER) == 1) {
        modbusTCP.holdingRegisterWrite(Reg::SAVE_TIMER, 0);
        calib.waitTime = waitTime;
        saveCalibration();
        Serial.print(F("Timer saved: ")); Serial.println(waitTime);
    }

    // ── Reset total counter ─────────────────────────────────────────
    if (modbusTCP.holdingRegisterRead(Reg::RESET_TOTAL) == 1) {
        modbusTCP.holdingRegisterWrite(Reg::RESET_TOTAL, 0);
        Serial.print(F("Total counter reset (was "));
        Serial.print(totalCounter); Serial.println(F(")"));
        totalCounter = 0;
        saveCounter();
    }

    // ── Save calibration ────────────────────────────────────────────
    if (modbusTCP.holdingRegisterRead(Reg::SAVE_CALIB) == 1) {
        modbusTCP.holdingRegisterWrite(Reg::SAVE_CALIB, 0);
        pullCalibrationFromRegisters();
        saveCalibration();
        Serial.println(F("Calibration saved"));
    }
}

void updateStatusRegisters() {
    modbusTCP.holdingRegisterWrite(Reg::STATE,          currentState);
    modbusTCP.holdingRegisterWrite(Reg::SPEED_SELECTED, speedSelected);
    modbusTCP.holdingRegisterWrite(Reg::REMAINING_TIME, remainingTime >= 0 ? remainingTime : 0);
    modbusTCP.holdingRegisterWrite(Reg::WAIT_TIME,      waitTime);
    modbusTCP.holdingRegisterWrite(Reg::CURRENT_CTR_L,  (uint16_t)(currentCounter & 0xFFFF));
    modbusTCP.holdingRegisterWrite(Reg::CURRENT_CTR_H,  (uint16_t)(currentCounter >> 16));
    modbusTCP.holdingRegisterWrite(Reg::TOTAL_CTR_L,    (uint16_t)(totalCounter & 0xFFFF));
    modbusTCP.holdingRegisterWrite(Reg::TOTAL_CTR_H,    (uint16_t)(totalCounter >> 16));
    modbusTCP.holdingRegisterWrite(Reg::SELECTED_TRAY,  traySelected);
    modbusTCP.holdingRegisterWrite(Reg::HEARTBEAT,      heartbeatToggle ? 1 : 0);
    modbusTCP.holdingRegisterWrite(Reg::INPUT_STATE,    prevInputs);
    modbusTCP.holdingRegisterWrite(Reg::OUTPUT_STATE,   outputState);
}

// =====================================================================
//  CALIBRATION ↔ MODBUS REGISTERS
// =====================================================================

/** Push current CalibrationData into Modbus holding registers
 *  so the HMI can display / edit them.  Factor × 1000 → uint16. */
void pushCalibrationToRegisters() {
    for (int m = 0; m < NUM_MOTORS; m++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            modbusTCP.holdingRegisterWrite(
                Reg::MOTOR_FACTORS_BASE + m * NUM_SPEEDS + s,
                (uint16_t)(calib.motorFactors[m][s] * 1000.0f));

    for (int t = 0; t < NUM_TRAYS; t++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            modbusTCP.holdingRegisterWrite(
                Reg::TRAY_TIME_BASE + t * NUM_SPEEDS + s,
                (uint16_t)(calib.trayTimeFactors[t][s] * 1000.0f));

    for (int t = 0; t < NUM_TRAYS; t++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            modbusTCP.holdingRegisterWrite(
                Reg::TRAY_M8_BASE + t * NUM_SPEEDS + s,
                (uint16_t)(calib.trayMotor8Factors[t][s] * 1000.0f));
}

/** Pull edited calibration values from Modbus registers back into
 *  CalibrationData struct.  uint16 / 1000.0 → factor. */
void pullCalibrationFromRegisters() {
    for (int m = 0; m < NUM_MOTORS; m++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            calib.motorFactors[m][s] =
                modbusTCP.holdingRegisterRead(
                    Reg::MOTOR_FACTORS_BASE + m * NUM_SPEEDS + s) / 1000.0f;

    for (int t = 0; t < NUM_TRAYS; t++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            calib.trayTimeFactors[t][s] =
                modbusTCP.holdingRegisterRead(
                    Reg::TRAY_TIME_BASE + t * NUM_SPEEDS + s) / 1000.0f;

    for (int t = 0; t < NUM_TRAYS; t++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            calib.trayMotor8Factors[t][s] =
                modbusTCP.holdingRegisterRead(
                    Reg::TRAY_M8_BASE + t * NUM_SPEEDS + s) / 1000.0f;

    // Re-apply motor speeds if currently running
    if (speedSelected >= 1 && traySelected >= 1) setMotorSpeeds();
}

// =====================================================================
//  FLASH PERSISTENCE
// =====================================================================

void loadCalibration() {
    CalibrationData stored = flashCalib.read();
    if (stored.magic == CALIB_MAGIC) {
        calib = stored;
        Serial.println(F("Calibration loaded from flash"));
    } else {
        loadFactoryDefaults(calib);
        Serial.println(F("No saved calibration — using factory defaults"));
    }
}

void saveCalibration() {
    calib.magic = CALIB_MAGIC;
    flashCalib.write(calib);
    Serial.println(F("Calibration saved to flash"));
}

void loadCounter() {
    CounterData cd = flashCounter.read();
    if (cd.magic == CALIB_MAGIC) {
        totalCounter = cd.totalCounter;
        Serial.print(F("Total counter loaded: ")); Serial.println(totalCounter);
    } else {
        totalCounter = 0;
        Serial.println(F("No saved counter — starting at 0"));
    }
}

void saveCounter() {
    CounterData cd;
    cd.magic        = CALIB_MAGIC;
    cd.totalCounter = totalCounter;
    flashCounter.write(cd);
}

// =====================================================================
//  PRODUCTION LOGGING  (Serial CSV — capture with terminal or logger)
// =====================================================================

void logProductionRun() {
    if (currentCounter == 0) return;

    // CSV: timestamp_ms, tray, speed, batchCount, totalCount
    Serial.print(F("LOG,"));
    Serial.print(millis());           Serial.print(',');
    if (traySelected >= 1 && traySelected <= NUM_TRAYS)
        Serial.print(TRAY_NAMES[traySelected - 1]);
    else
        Serial.print(F("none"));
    Serial.print(',');
    Serial.print(speedSelected);      Serial.print(',');
    Serial.print(currentCounter);     Serial.print(',');
    Serial.println(totalCounter);
}
