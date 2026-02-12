#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief Run2 State - Main Production Run
 * 
 * This is the primary production state where:
 * - All 8 conveyor motors are running
 * - Plant counting is active (sensor-based)
 * - Green indicator light shows system running
 * - Production data is logged when stopped
 * 
 * Entry conditions:
 * - From TimeDelay state after countdown completes
 * - From E-stop state (resuming production)
 * 
 * Motor activation:
 * - Motors 1-6: All conveyor motors activated
 * - Motor 8: Upper soil belt (tray-specific calibration)
 * NOTE: All 8 motors now activated (fixed bug where motors 5&6 were missing)
 * 
 * Counter behavior:
 * - Counter continues from previous value (no reset on E-stop recovery)
 * - Timer interval calculated based on tray type and speed setting
 * - Batched disk writes (every 10 counts) for performance
 */
void MainWindow::stateRun2()
{
	//Enter Run2 State
	//In Run2 State - all motors running
	//Smash Button timer out
	//Counter was reset to 0 if countdown timer timed out
	//Green light on

	previousState = currentState;
	currentState = Run2State;
	qInfo() << "Run2State";

	// Note: Counter NOT reset here - allows resuming after E-stop
	// Counter only reset when TimeDelay completes (new production run)


	//Set motor output on (all 8 motors)
	writeDigitalOutput(m_motor1DigitalOutAddress, 1);
	writeDigitalOutput(m_motor2DigitalOutAddress, 1);
	writeDigitalOutput(m_motor3DigitalOutAddress, 1);
	writeDigitalOutput(m_motor4DigitalOutAddress, 1);
	writeDigitalOutput(m_motor5DigitalOutAddress, 1);  // Fixed: was missing
	writeDigitalOutput(m_motor6DigitalOutAddress, 1);  // Fixed: was missing
    writeDigitalOutput(m_motor8DigitalOutAddress, 1);


	//Start	counter timer (interval calculated from tray factors and speed)
	timerCounter.start(static_cast<int>(counterTimerInterval * 1000.0));

	//Set green light on (production active)
	writeDigitalOutput(m_greenLightDigitalOutAddress, 1);
	writeDigitalOutput(m_yellowLightDigitalOutAddress, 0);
	writeDigitalOutput(m_redLightDigitalOutAddress, 0);

}

void MainWindow::stateRun2UpdateTimer()
{
    if(timerCounter.isActive())
    {
        timerCounter.stop();
        timerCounter.start(static_cast<int>(counterTimerInterval * 1000.0));
    }

}

