#include "scaninputs.h"
#include <QDebug>
#include "motor.h"

ScanInputs::ScanInputs()
{
	modbusClient = nullptr;
	m_timer = nullptr;
	m_address = 2;
}

// ScanInputs::ScanInputs(QSharedPointer<QModbusRtuSerialClient> serialClient, int analogAddress)
//     : modbusClient{ serialClient }, m_address{ analogAddress }
// {
//     //modbusClient = nullptr;
//     m_timer = nullptr;
//     //modbusClient = serialClient;
//     //m_address = analogAddress;
//     connected = true;
// }

ScanInputs::~ScanInputs()
{
	qInfo() << "ScanInputs Destructor";
}

void ScanInputs::connectModbus(QString port)
{
	//enable modbus logging
	//QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));

	m_portName = port;

	modbusClient = QSharedPointer<QModbusRtuSerialClient>::create();

	modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
		m_portName);
	modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter,
		QSerialPort::NoParity);
	modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
		QSerialPort::Baud57600);
	modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
		QSerialPort::Data8);
	modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
		QSerialPort::OneStop);

	if (!modbusClient->connectDevice())
	{
		qWarning() << "Error Connecting to Modbus Device";
	}
	else
		connected = true;
}

void ScanInputs::setComPort(QString port)
{
	m_portName = port;
	if(connected)
	{
		modbusClient->disconnectDevice();
		connectModbus(m_portName);
	}
	
}

void ScanInputs::onReplyFinished()
{
	QModbusReply* reply = qobject_cast<QModbusReply*>(sender());
	if (!reply)
	{
		return;
	}

	// Check if the reply is finished and has no error
	if (reply->isFinished() && reply->error() == QModbusDevice::NoError) {
		// Get the result data unit from the reply
		QModbusDataUnit result = reply->result();

		// Loop through the values and print the coil status
		for (int i = 0; i < result.valueCount(); i++) {
			bool coilStatus = result.value(i); // Get the value as a bool
			//qDebug() << "Coil" << i << "status:" << coilStatus;

			//TODO Need to test
			if (coilStatus != inputCache[i])
			{
				emit inputChanged(i, coilStatus);
				inputCache[i] = coilStatus;
				qInfo() << "Input" << i << "changed to" << coilStatus;
			}
		}
	}
	else {
		// Handle the error
		qDebug() << "Read error:" << reply->errorString();
	}

	// Delete the reply object
	reply->deleteLater();
}

void ScanInputs::timeout()
{
	//qInfo() << "ScanInputs timer timeout";
	// Send the read request and get a QModbusReply object
	QModbusReply const* reply = modbusClient->sendReadRequest(request, m_address);

	// Check if the reply is valid
	if (reply) {
		// Connect a slot to the finished signal of the reply
		connect(reply, &QModbusReply::finished, this, &ScanInputs::onReplyFinished);
	}
	else {
		// Handle the error
		qWarning() << "Send error:" << modbusClient->errorString();
	}
}

void ScanInputs::run()
{
	this->connectModbus(m_portName);

	request.setRegisterType(QModbusDataUnit::DiscreteInputs);
	request.setStartAddress(0);
	request.setValueCount(m_inputCount);  // Read 10 coils starting from analogAddress 0

	inputCache.resize(request.valueCount());
	inputCache.fill(true);

	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &ScanInputs::timeout);
	m_timer->setInterval(m_timerDelay);
	if (connected)
	{
		m_timer->start();
	}
}

void ScanInputs::stop()
{
	m_timer->stop();
}
