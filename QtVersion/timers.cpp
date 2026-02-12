#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::countDownTimerDecrement()
{
	//Motor Timer control - decrement
	remainingTime--;
	ui->labelTimer->setText(QString::number(remainingTime));
	if (remainingTime == 3)
	{
		//Ring buzzer
		qInfo() << "Ring buzzer";
		writeDigitalOutput(m_buzzerDigitalOutAddress, 1);

	}
	else if (remainingTime == 1)
	{
		//Turn buzzer off
		writeDigitalOutput(m_buzzerDigitalOutAddress, 0);
	}
	else if (remainingTime <= 0)
	{
		timerMotors.stop();
		ui->labelTimer->setText("0");

		StateTimeDelayTimerComplete();
	}
}

void MainWindow::on_pushButtonPlus5_clicked()
{
	adjustWaitTime(5);
}

void MainWindow::on_pushButtonPlus1_clicked()
{
	adjustWaitTime(1);
}

void MainWindow::on_pushButtonSaveTimer_clicked()
{
	writeTimerJson(waitTime);
	ui->pushButtonSaveTimer->setEnabled(false);
}

void MainWindow::on_pushButtonMinus1_clicked()
{
	if (waitTime >= 1)
		adjustWaitTime(-1);
}

void MainWindow::on_pushButtonMinus5_clicked()
{
	if (waitTime >= 5)
		adjustWaitTime(-5);
}

void MainWindow::adjustWaitTime(int adjustment)
{
	ui->pushButtonSaveTimer->setEnabled(true);
	waitTime = waitTime + adjustment;
	remainingTime = waitTime;
	ui->labelTimer->setText(QString::number(remainingTime));
}


