#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::stateRun1()
{
	//Enter Run1 State
	//In Run1 State - ALl motors start - counting begins - there may be trays on belt one ready to run
    //Yellow light on - Pete requested to change to Green light on march 2025
    //Green light on

	previousState = currentState;
	currentState = Run1State;
	qInfo() << "Run1State";

	//Start counter
	timerCounter.start(counterTimerInterval * 1000);

	//Set motors output on - plus extra motors if needed
	writeDigitalOutput(m_motor1DigitalOutAddress, 1);
	writeDigitalOutput(m_motor2DigitalOutAddress, 1);
	writeDigitalOutput(m_motor3DigitalOutAddress, 1);
	writeDigitalOutput(m_motor4DigitalOutAddress, 1);
	writeDigitalOutput(m_motor5DigitalOutAddress, 1);
	writeDigitalOutput(m_motor6DigitalOutAddress, 1);
    writeDigitalOutput(m_motor8DigitalOutAddress, 1);


	//Turn yellow light on
    writeDigitalOutput(m_yellowLightDigitalOutAddress, 0);
    writeDigitalOutput(m_greenLightDigitalOutAddress, 1);
	writeDigitalOutput(m_redLightDigitalOutAddress, 0);

	//disable timer adjust while running
	ui->pushButtonMinus1->setEnabled(false);
	ui->pushButtonPlus1->setEnabled(false);
	ui->pushButtonMinus5->setEnabled(false);
	ui->pushButtonPlus5->setEnabled(false);
	ui->pushButtonSaveTimer->setEnabled(false);
}
