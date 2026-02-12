#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::stateTimeDelay()
{
	//Timer functions are located in timers.cpp

	//Timer Running
	//Yellow Light is on
	//Buzzer rings 3 seconds before timeout and lasts for 1 second
	//Counter stays on screen until timout and then resets to 0

	previousState = currentState;
	currentState = states::TimeDelayState;
	qInfo() << "TimeDelayState";

	timerMotors.start(1000); // 1 second interval

	//Stop Motors 1 and 2
	//This might be dependent on previous state
	//Time delay can happen from Run1 or Run2
	//Does the same thing happen from both states?
	writeDigitalOutput(m_motor1DigitalOutAddress, 0);
	writeDigitalOutput(m_motor2DigitalOutAddress, 0);
    writeDigitalOutput(m_motor8DigitalOutAddress, 0);

	//Set light to Yellow
	writeDigitalOutput(m_yellowLightDigitalOutAddress, 1);
	writeDigitalOutput(m_greenLightDigitalOutAddress, 0);
	writeDigitalOutput(m_redLightDigitalOutAddress, 0);

	//Make sure counterTimer is not running
	if (timerCounter.isActive())
	{
		timerCounter.stop();
	}
}

void MainWindow::StateTimeDelayTimerComplete()
{
	// Log production run before resetting counter
	logProductionRun();

	//update current counter file on start
	m_currentCounter = 0;
	ui->lcdCurrentCounter->display(m_currentCounter);

	//Enter Run2 State
	stateRun2();
}
