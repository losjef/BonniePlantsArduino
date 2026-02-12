#include "motor.h"

Motor::Motor(QObject *parent)
    : QObject{parent}
{

}

Motor::Motor(QList<double> speedCalFactors)
{
    m_speedCalFactor1 = speedCalFactors[0];
    m_speedCalFactor2 = speedCalFactors[1];
    m_speedCalFactor3 = speedCalFactors[2];
    m_speedCalFactor4 = speedCalFactors[3];
    m_speedCalFactor5 = speedCalFactors[4];
    m_speedCalFactor6 = speedCalFactors[5];
}

double Motor::speedCalFactor1() const
{
    return m_speedCalFactor1;
}

void Motor::setSpeedCalFactor1(double newSpeedCalFactor1)
{
    if (qFuzzyCompare(m_speedCalFactor1, newSpeedCalFactor1))
        return;
    m_speedCalFactor1 = newSpeedCalFactor1;
    emit speedCalFactor1Changed();
}

double Motor::speedCalFactor2() const
{
    return m_speedCalFactor2;
}

void Motor::setSpeedCalFactor2(double newSpeedCalFactor2)
{
    if (qFuzzyCompare(m_speedCalFactor2, newSpeedCalFactor2))
        return;
    m_speedCalFactor2 = newSpeedCalFactor2;
    emit speedCalFactor2Changed();
}

double Motor::speedCalFactor3() const
{
    return m_speedCalFactor3;
}

void Motor::setSpeedCalFactor3(double newSpeedCalFactor3)
{
    if (qFuzzyCompare(m_speedCalFactor3, newSpeedCalFactor3))
        return;
    m_speedCalFactor3 = newSpeedCalFactor3;
    emit speedCalFactor3Changed();
}

double Motor::speedCalFactor4() const
{
    return m_speedCalFactor4;
}

void Motor::setSpeedCalFactor4(double newSpeedCalFactor4)
{
    if (qFuzzyCompare(m_speedCalFactor4, newSpeedCalFactor4))
        return;
    m_speedCalFactor4 = newSpeedCalFactor4;
    emit speedCalFactor4Changed();
}

double Motor::speedCalFactor5() const
{
    return m_speedCalFactor5;
}

void Motor::setSpeedCalFactor5(double newSpeedCalFactor5)
{
    if (qFuzzyCompare(m_speedCalFactor5, newSpeedCalFactor5))
        return;
    m_speedCalFactor5 = newSpeedCalFactor5;
    emit speedCalFactor5Changed();
}

double Motor::speedCalFactor6() const
{
    return m_speedCalFactor6;
}

void Motor::setSpeedCalFactor6(double newSpeedCalFactor6)
{
    if (qFuzzyCompare(m_speedCalFactor6, newSpeedCalFactor6))
        return;
    m_speedCalFactor6 = newSpeedCalFactor6;
    emit speedCalFactor6Changed();
}

QString Motor::name() const
{
    return m_name;
}

void Motor::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

double Motor::speed() const
{
    return m_speed;
}

void Motor::setSpeed(double newSpeed)
{
    m_speed = newSpeed;
}

int Motor::analogAddress() const
{
    return m_analogAddress;
}

void Motor::setAnalogAddress(int newAddress)
{
  m_analogAddress = newAddress;
}

void Motor::setSpeedCalFactors(QList<double> speedCalFactors)
{
    m_speedCalFactor1 = speedCalFactors[0];
    m_speedCalFactor2 = speedCalFactors[1];
    m_speedCalFactor3 = speedCalFactors[2];
    m_speedCalFactor4 = speedCalFactors[3];
    m_speedCalFactor5 = speedCalFactors[4];
    m_speedCalFactor6 = speedCalFactors[5];
    emit speedCalFactor1Changed();
    emit speedCalFactor2Changed();
    emit speedCalFactor3Changed();
    emit speedCalFactor4Changed();
    emit speedCalFactor5Changed();
    emit speedCalFactor6Changed();
}

int Motor::digitalOutAddress() const
{
  return m_digitalAddress;
}

void Motor::setDigitalAddress(int m_digital_address)
{
    m_digitalAddress = m_digital_address;
}

int Motor::modbusDigitalDeviceID() const
{
    return m_modbusDigitalDeviceID;
}

void Motor::setModbusDigitalDeviceID(int newModbusDeviceID)
{
    m_modbusDigitalDeviceID = newModbusDeviceID;
}

int Motor::modbusAnalogDeviceID() const
{
    return m_modbusAnalogDeviceID;
}

void Motor::setModbusAnalogDeviceID(int newModbusAnalogDeviceID)
{
    m_modbusAnalogDeviceID = newModbusAnalogDeviceID;
}

