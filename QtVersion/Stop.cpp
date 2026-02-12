#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::stateStop()
{
	//Red Light on

	QMutexLocker locker(&m_stateMutex);  // Thread-safe state change

	previousState = currentState;
	currentState = states::StopState;
	qInfo() << "StopState";

	// Save counter to disk when stopping (safety measure)
	if (m_countersSinceLastWrite > 0) {
		saveCounterToDisk();
		m_countersSinceLastWrite = 0;
	}

	//stop motors
	writeDigitalOutput(m_motor1DigitalOutAddress, 0);
	writeDigitalOutput(m_motor2DigitalOutAddress, 0);
	writeDigitalOutput(m_motor3DigitalOutAddress, 0);
	writeDigitalOutput(m_motor4DigitalOutAddress, 0);
	writeDigitalOutput(m_motor5DigitalOutAddress, 0);
	writeDigitalOutput(m_motor6DigitalOutAddress, 0);
    writeDigitalOutput(m_motor8DigitalOutAddress, 0);

	//Set light to red
	writeDigitalOutput(m_redLightDigitalOutAddress, 1);
	writeDigitalOutput(m_yellowLightDigitalOutAddress, 0);
	writeDigitalOutput(m_greenLightDigitalOutAddress, 0);

	//stop timer and reset
	timerMotors.stop();
	timerCounter.stop();
	remainingTime = waitTime;
	ui->labelTimer->setText(QString::number(remainingTime));

	ui->pushButtonMinus1->setEnabled(true);
	ui->pushButtonPlus1->setEnabled(true);
	ui->pushButtonMinus5->setEnabled(true);
	ui->pushButtonPlus5->setEnabled(true);
	ui->pushButtonSaveTimer->setEnabled(true);
}
