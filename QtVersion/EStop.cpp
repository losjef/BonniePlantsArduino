#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief Emergency Stop State Handler
 * 
 * CRITICAL SAFETY FUNCTION - Immediately halts all conveyor operations.
 * 
 * Actions performed:
 * 1. Lock state mutex to prevent race conditions
 * 2. Record previous state for recovery after E-stop cleared
 * 3. IMMEDIATELY save counter to disk (data protection)
 * 4. Turn red light ON, yellow/green OFF
 * 5. Turn OFF all motor digital outputs
 * 6. Activate E-stop output signal
 * 7. Stop counter timer
 * 8. Update UI indicators
 * 
 * Thread Safety: Uses QMutexLocker for atomic state transition
 * Data Safety: Forces immediate counter write even if < batch size
 * Recovery: Preserves estopReturnToState for resumption after clear
 */
void MainWindow::stateEstop()
{
	// Acquire mutex lock - released automatically when locker goes out of scope
	QMutexLocker locker(&m_stateMutex);  // Thread-safe state change

	estopReturnToState = previousState; //Need to record previous state to return to when estop is cleared
	previousState = currentState;
	currentState = states::EstopState;
	qCritical() << "E-STOP ACTIVATED - Emergency shutdown initiated";

	// CRITICAL: Save counter immediately during E-stop
	if (m_countersSinceLastWrite > 0) {
		saveCounterToDisk();
		m_countersSinceLastWrite = 0;
	}

	//Set light to red
	writeDigitalOutput(m_redLightDigitalOutAddress, 1);
	writeDigitalOutput(m_yellowLightDigitalOutAddress, 0);
	writeDigitalOutput(m_greenLightDigitalOutAddress, 0);


	//Set motor output off
	writeDigitalOutput(m_motor1DigitalOutAddress, 0);
	writeDigitalOutput(m_motor2DigitalOutAddress, 0);
	writeDigitalOutput(m_motor3DigitalOutAddress, 0);
	writeDigitalOutput(m_motor4DigitalOutAddress, 0);
	writeDigitalOutput(m_motor5DigitalOutAddress, 0);
	writeDigitalOutput(m_motor6DigitalOutAddress, 0);
    writeDigitalOutput(m_motor8DigitalOutAddress, 0);

	//Set EStop output on
	writeDigitalOutput(m_EStopOutDigitalOutAddress, 1);

	//stop timer and reset
	timerMotors.stop();
	timerCounter.stop();
}

void MainWindow::StateEstopCleared()
{
    if(previousState == states::StopState)
            {
                writeDigitalOutput(m_EStopOutDigitalOutAddress, 0);
                stateStop();
            }
        else if (previousState == states::Run1State)
            {
                    writeDigitalOutput(m_EStopOutDigitalOutAddress, 0);
                    estopReturnToState = states::Run1State;
                    stateBuzzerDelay();
            }
        else if (previousState == states::TimeDelayState)
            {
                writeDigitalOutput(m_EStopOutDigitalOutAddress, 0);
                stateTimeDelay();
            }
        else if (previousState == states::Run2State)
            {
                writeDigitalOutput(m_EStopOutDigitalOutAddress, 0);
                estopReturnToState = states::Run2State;
                stateBuzzerDelay();
            }
        else
            {
                writeDigitalOutput(m_EStopOutDigitalOutAddress, 0);
            }

	//stateStop();  //This might not be right since counters will be reset - do we want to go to timedelay state?
	// Do I need to add the tracking of previous state?
	// This would allow entering stop state from estop state without reseting counters
}

