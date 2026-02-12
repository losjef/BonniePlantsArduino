#include "ModbusInput.h"

ModbusInput::ModbusInput(QObject *parent)
	: QObject(parent)
{}

ModbusInput::~ModbusInput()
{}

int ModbusInput::getInputAddress() const
{
    return m_inputAddress;
}

void ModbusInput::setInputAddress(int newInputAddress)
{
    m_inputAddress = newInputAddress;
}

int ModbusInput::getModbusDeviceId() const
{
    return m_modbusDeviceID;
}

void ModbusInput::setModbusDeviceId(int newModbusDeviceID)
{
    m_modbusDeviceID = newModbusDeviceID;
}

quint16 ModbusInput::getInputStatus() const
{
    return m_inputStatus;
}

void ModbusInput::setInputStatus(quint16 newInputStatus)
{
	m_inputStatus = newInputStatus;
    if(m_inputStatus != m_cachedStatus)
    {
        m_cachedStatus = m_inputStatus;
    	emit inputStatusChanged(m_inputStatus);
	}
}
