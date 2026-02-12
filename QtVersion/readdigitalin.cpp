#include "mainwindow.h"

int MainWindow::readDigitalInputs()
{
	// Read the input registers
	QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs, 0, 8);
	if (auto* reply = modbusClient1->sendReadRequest(readUnit, 1))
	{
		if (!reply->isFinished())
		{
			connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
		}
		else
		{
			delete reply; // broadcast replies return immediately
		}
	}
	else
	{
		qDebug() << "Read error: " << modbusClient1->errorString();
	}
	return 0;
}

void MainWindow::readReady()
{
	auto reply = qobject_cast<QModbusReply*>(sender());
	if (!reply)
		return;

	if (reply->error() == QModbusDevice::NoError) {
		const QModbusDataUnit unit = reply->result();
		for (qsizetype i = 0, total = unit.valueCount(); i < total; ++i) {
			const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i).arg(QString::number(unit.value(i), unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
			qDebug() << entry;

			switch (i)
			{
			case 0:
				startButton.setInputStatus(unit.value(i));
				break;
			case 1:
				stopButton.setInputStatus(unit.value(i));
				break;
			case 2:
				startDelayButton.setInputStatus(unit.value(i));
				break;
			case 3:
				eStopButton.setInputStatus(unit.value(i));
				break;
			default:
				break;
			}
		}
	}
	else if (reply->error() == QModbusDevice::ProtocolError) {
		qDebug() << "Read response protocol error";
	}
	else {
		qDebug() << "Read response error";
	}

	reply->deleteLater();
}