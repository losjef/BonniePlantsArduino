#include "ModbusOutput.h"
#include <QDebug>

// Might be better to be called ModbusDigitalOutput
// Analog output is left in motor class for now - it does not require cache checking or special funtions
// It appears that the ModbusOutput class would be required since it will allow for cache checking and emit the changed
// signal when the output status is changed

ModbusOutput::ModbusOutput(QObject *parent)
	: QObject(parent)
{}

ModbusOutput::~ModbusOutput()
{}

quint16 ModbusOutput::getOutputStatus() const
{
	return m_outputStatus;
}

void ModbusOutput::setOutputStatus(quint16 mOutputStatus)
{
	m_mutex.lock();
	m_outputStatus = mOutputStatus;
	//When output status is set, it should be compared to the cached status
	//If the status is different, then the output should be written and the cached status updated and emit outputStatusChanged
	if (m_outputStatus != m_cachedStatus)
	{
		emit outputStatusChanged();
		m_cachedStatus = m_outputStatus;
	}
	m_mutex.unlock();
}

int ModbusOutput::getOutputAddress() const
{
	return m_outputAddress;
}

void ModbusOutput::setOutputAddress(int mOutputAddress)
{
	m_mutex.lock();
	m_outputAddress = mOutputAddress;
	m_mutex.unlock();
}

int ModbusOutput::getModbusDeviceId() const
{
	return m_modbusDeviceID;
}

void ModbusOutput::setModbusDeviceId(int mModbusDeviceId)
{
	m_mutex.lock();
	m_modbusDeviceID = mModbusDeviceId;
	m_mutex.unlock();
}


