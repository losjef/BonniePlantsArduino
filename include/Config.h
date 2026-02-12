#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file Config.h
 * @brief Hardware configuration, register map, and defaults for BonnieConveyor
 *
 * This file is the single source of truth for:
 *   - P1AM slot / channel assignments
 *   - Modbus TCP register map (exposed to CM5-T15W HMI)
 *   - System constants and state definitions
 *   - Calibration data structures and factory defaults
 *
 * Edit the values in this file to match your wiring, then re-upload.
 */

#include <cstdint>

// =========================================================================
// P1AM Module Slot Assignments (left to right from CPU in P1-01AC base)
// =========================================================================
constexpr uint8_t SLOT_DI  = 1;   // P1-16ND3  16-pt DC Input
constexpr uint8_t SLOT_DO  = 2;   // P1-16TR   16-pt Relay Output
constexpr uint8_t SLOT_AO  = 3;   // P1-08DAL-2  8-ch Analog 0-10 V Output

// =========================================================================
// Discrete Input Bit Positions  (P1-16ND3  —  Slot 1)
// P1AM.readDiscrete(SLOT_DI) returns uint32_t bitmask, bit 0 = ch 1
// =========================================================================
constexpr uint8_t BIT_START       = 0;  // Ch 1 — Start        (NO)
constexpr uint8_t BIT_STOP        = 1;  // Ch 2 — Stop         (NC, active LOW)
constexpr uint8_t BIT_START_DELAY = 2;  // Ch 3 — Start Delay  (NO)
constexpr uint8_t BIT_ESTOP       = 3;  // Ch 4 — E-Stop       (NC, active LOW)
// Channels 5-16 available for expansion

// =========================================================================
// Relay Output Bit Positions  (P1-16TR  —  Slot 2)
// Written as a uint16_t bitmask via P1AM.writeDiscrete(SLOT_DO, mask)
// =========================================================================
constexpr uint8_t BIT_MOTOR1      = 0;  // Ch 1  — Infeed Belt
constexpr uint8_t BIT_MOTOR2      = 1;  // Ch 2  — Lower Soil Belt
constexpr uint8_t BIT_MOTOR3      = 2;  // Ch 3  — Flat Filler Belt
constexpr uint8_t BIT_MOTOR4      = 3;  // Ch 4  — Planting Line Belt
constexpr uint8_t BIT_MOTOR5      = 4;  // Ch 5  — Motor 5
constexpr uint8_t BIT_MOTOR6      = 5;  // Ch 6  — Motor 6
//                                  6   // Ch 7  — (unused)
constexpr uint8_t BIT_MOTOR8      = 7;  // Ch 8  — Upper Soil Belt
constexpr uint8_t BIT_RED_LIGHT   = 8;  // Ch 9  — Red indicator
constexpr uint8_t BIT_YEL_LIGHT   = 9;  // Ch 10 — Yellow indicator
constexpr uint8_t BIT_GRN_LIGHT   = 10; // Ch 11 — Green indicator
constexpr uint8_t BIT_BUZZER      = 11; // Ch 12 — Buzzer
constexpr uint8_t BIT_ESTOP_OUT   = 12; // Ch 13 — E-Stop output relay
// Channels 14-16 available for expansion

// All motor relay bits ORed together (for quick all-on / all-off)
constexpr uint16_t MOTOR_ALL_MASK = (1u << BIT_MOTOR1) | (1u << BIT_MOTOR2) |
                                    (1u << BIT_MOTOR3) | (1u << BIT_MOTOR4) |
                                    (1u << BIT_MOTOR5) | (1u << BIT_MOTOR6) |
                                    (1u << BIT_MOTOR8);

// Motors 1, 2, 8 — stopped during TimeDelay state
constexpr uint16_t MOTOR_PARTIAL_MASK = (1u << BIT_MOTOR1) | (1u << BIT_MOTOR2) |
                                        (1u << BIT_MOTOR8);

// =========================================================================
// Analog Output Channels  (P1-08DAL-2  —  Slot 3)
// P1AM.writeAnalog(SLOT_AO, channel, value)   channel is 1-based
// 12-bit DAC: 0-4095 → 0.0 V – 10.0 V
// =========================================================================
constexpr uint8_t ACH_MOTOR1 = 1;  // Ch 1 — Infeed Belt speed
constexpr uint8_t ACH_MOTOR2 = 2;  // Ch 2 — Lower Soil Belt speed
constexpr uint8_t ACH_MOTOR3 = 3;  // Ch 3 — Flat Filler Belt speed
constexpr uint8_t ACH_MOTOR4 = 4;  // Ch 4 — Planting Line Belt speed
constexpr uint8_t ACH_MOTOR5 = 5;  // Ch 5 — Motor 5 speed
constexpr uint8_t ACH_MOTOR6 = 6;  // Ch 6 — Motor 6 speed
//                             7   // Ch 7 — (unused)
constexpr uint8_t ACH_MOTOR8 = 8;  // Ch 8 — Upper Soil Belt speed

constexpr int     DAC_MAX    = 4095;  // 12-bit full scale

inline int percentToDAC(float pct) {
    if (pct < 0.0f) pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;
    return static_cast<int>(pct / 100.0f * DAC_MAX);
}

// =========================================================================
// Motor / Tray Counts
// =========================================================================
constexpr int NUM_MOTORS = 7;   // Motors 1-6 + Motor 8 (index 6)
constexpr int NUM_SPEEDS = 6;   // Speed settings 1-6
constexpr int NUM_TRAYS  = 6;   // Tray types

constexpr int MOTOR_8_IDX = 6;  // Motor 8 lives at array index 6

// Motor definitions:  { relay bit, analog channel (1-based) }
struct MotorDef {
    uint8_t relayBit;
    uint8_t analogCh;
};

constexpr MotorDef MOTOR_DEFS[NUM_MOTORS] = {
    { BIT_MOTOR1, ACH_MOTOR1 },   // idx 0 — Motor 1 Infeed Belt
    { BIT_MOTOR2, ACH_MOTOR2 },   // idx 1 — Motor 2 Lower Soil Belt
    { BIT_MOTOR3, ACH_MOTOR3 },   // idx 2 — Motor 3 Flat Filler Belt
    { BIT_MOTOR4, ACH_MOTOR4 },   // idx 3 — Motor 4 Planting Line Belt
    { BIT_MOTOR5, ACH_MOTOR5 },   // idx 4 — Motor 5
    { BIT_MOTOR6, ACH_MOTOR6 },   // idx 5 — Motor 6
    { BIT_MOTOR8, ACH_MOTOR8 },   // idx 6 — Motor 8 Upper Soil Belt
};

// Tray names (for serial logging only)
constexpr const char* TRAY_NAMES[NUM_TRAYS] = {
    "6-06", "3.5", "4.5", "5", "Gallon", "8"
};

// =========================================================================
// System State Enumeration
// =========================================================================
enum SystemState : uint8_t {
    STATE_STOP         = 0,
    STATE_RUN1         = 1,
    STATE_TIME_DELAY   = 2,
    STATE_RUN2         = 3,
    STATE_ESTOP        = 4,
    STATE_BUZZER_DELAY = 5
};

// =========================================================================
// System Constants
// =========================================================================
constexpr float    BASE_SPEED_PCT       = 100.0f;  // 100 % = 10 V
constexpr int      DEFAULT_WAIT_TIME    = 60;       // seconds
constexpr int      BUZZER_PRE_SEC       = 3;        // buzzer duration (sec)
constexpr int      COUNTER_BATCH_SIZE   = 100;      // flash writes per N counts
constexpr uint32_t CALIB_MAGIC          = 0xBEEFCAFEu;
constexpr unsigned long SCAN_CYCLE_MS   = 20;       // main loop target period
constexpr unsigned long HEARTBEAT_MS    = 1000;

// =========================================================================
// Network Configuration  (P1AM-ETH shield, WIZnet W5500)
// =========================================================================
constexpr uint8_t  ETH_CS_PIN = 5;    // P1AM-ETH chip-select pin
// Update the MAC to match the sticker on your P1AM-ETH shield
constexpr uint8_t  DEFAULT_MAC[6] = { 0x60, 0x52, 0xD0, 0x06, 0x70, 0x27 };

// Default static IP — change to match your plant network
constexpr uint8_t  DEFAULT_IP[4]      = { 192, 168, 1, 100 };
constexpr uint8_t  DEFAULT_SUBNET[4]  = { 255, 255, 255, 0 };
constexpr uint8_t  DEFAULT_GATEWAY[4] = { 192, 168, 1, 1 };
constexpr int      MODBUS_TCP_PORT    = 502;

// =========================================================================
// Modbus TCP Register Map  (Holding Registers — FC 03 / 06)
//
// The CM5-T15W C-more HMI connects as a Modbus TCP client to the
// P1AM's Modbus TCP server.  All data exchange goes through these
// holding registers.
// =========================================================================

// --- Status Block (Arduino writes, HMI reads) ---
namespace Reg {
    constexpr int STATE             = 0;   // SystemState enum value
    constexpr int SPEED_SELECTED    = 1;   // 1-6 (0 = none)
    constexpr int REMAINING_TIME    = 2;   // countdown seconds remaining
    constexpr int WAIT_TIME         = 3;   // configured wait time
    constexpr int CURRENT_CTR_L     = 4;   // current batch counter (low 16)
    constexpr int CURRENT_CTR_H     = 5;   // current batch counter (high 16)
    constexpr int TOTAL_CTR_L       = 6;   // lifetime counter (low 16)
    constexpr int TOTAL_CTR_H       = 7;   // lifetime counter (high 16)
    constexpr int SELECTED_TRAY     = 8;   // 1-6 (0 = none)
    constexpr int HEARTBEAT         = 9;   // toggles each second
    constexpr int INPUT_STATE       = 10;  // raw P1-16ND3 bitmask
    constexpr int OUTPUT_STATE      = 11;  // raw P1-16TR  bitmask

    // --- Command Block (HMI writes, Arduino reads & clears) ---
    constexpr int COMMAND           = 100; // one-shot command code
    constexpr int SPEED_SELECT      = 101; // speed selection (persistent)
    constexpr int TRAY_SELECT       = 102; // tray  selection (persistent)
    constexpr int TIMER_ADJUST      = 103; // signed +/-  (one-shot)
    constexpr int SAVE_TIMER        = 104; // 1 = save    (one-shot)
    constexpr int RESET_TOTAL       = 105; // 1 = reset   (one-shot)

    // --- Calibration Block (read/write from HMI) ---
    //  Motor factors : 200 + motorIdx*6 + speedIdx   (42 regs, 200-241)
    //  Tray time     : 250 + trayIdx*6  + speedIdx   (36 regs, 250-285)
    //  Tray motor8   : 300 + trayIdx*6  + speedIdx   (36 regs, 300-335)
    constexpr int MOTOR_FACTORS_BASE  = 200;
    constexpr int TRAY_TIME_BASE      = 250;
    constexpr int TRAY_M8_BASE        = 300;
    constexpr int SAVE_CALIB          = 400; // 1 = save calibration to flash

    constexpr int TOTAL_REGISTERS     = 402; // 0-401 inclusive
}

// Command codes (written to Reg::COMMAND by HMI)
namespace Cmd {
    constexpr int NONE            = 0;
    constexpr int START           = 1;
    constexpr int STOP            = 2;
    constexpr int START_DELAY     = 3;
    constexpr int ESTOP           = 4;
    constexpr int ESTOP_CLEAR     = 5;
    constexpr int RESET_CURRENT   = 6;
}

// =========================================================================
// Calibration Data (persisted in SAMD Flash via FlashStorage)
// =========================================================================
struct CalibrationData {
    uint32_t magic;                          // CALIB_MAGIC when valid
    float    motorFactors[NUM_MOTORS][NUM_SPEEDS];
    float    trayTimeFactors[NUM_TRAYS][NUM_SPEEDS];
    float    trayMotor8Factors[NUM_TRAYS][NUM_SPEEDS];
    int      waitTime;
};

struct CounterData {
    uint32_t magic;
    uint32_t totalCounter;
};

// =========================================================================
// Factory-Default Calibration Values
// =========================================================================

// All motor speed factors default to 1.0 (100 % of base speed)
inline void loadFactoryDefaults(CalibrationData& d) {
    d.magic = CALIB_MAGIC;
    for (int m = 0; m < NUM_MOTORS; m++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            d.motorFactors[m][s] = 1.0f;

    // Tray time factors (seconds per plant count) — defaults
    // These should be tuned per plant/tray combination on-site
    for (int t = 0; t < NUM_TRAYS; t++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            d.trayTimeFactors[t][s] = 2.0f;   // 2 seconds per count

    // Tray-specific Motor 8 speed factors — defaults
    for (int t = 0; t < NUM_TRAYS; t++)
        for (int s = 0; s < NUM_SPEEDS; s++)
            d.trayMotor8Factors[t][s] = 1.0f;

    d.waitTime = DEFAULT_WAIT_TIME;
}

#endif // CONFIG_H
