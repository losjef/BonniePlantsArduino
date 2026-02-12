#pragma once

#include <QObject>

class ModbusInput  : public QObject
{
	Q_OBJECT

public:
	ModbusInput(QObject *parent = nullptr);
	~ModbusInput();

    int getInputAddress() const;
    void setInputAddress(int newInputAddress);

    int getModbusDeviceId() const;
    void setModbusDeviceId(int newModbusDeviceID);

    quint16 getInputStatus() const;
    void setInputStatus(quint16 newInputStatus);

private:
	quint16 m_inputStatus{ 0 };
	quint16 m_cachedStatus{ false };
	int m_inputAddress{ 0 };
	int m_modbusDeviceID{ 0 };

signals:
    void inputStatusChanged(quint16 onOff);
};
