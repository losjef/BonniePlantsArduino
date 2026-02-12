#include "tray.h"

Tray::Tray(QObject *parent)
    : QObject{parent}
{

}

double Tray::getSpeed1TimeFactor() const {
    return speed1TimeFactor;
}

void Tray::setSpeed1TimeFactor(double speed1TimeFactor) {
    Tray::speed1TimeFactor = speed1TimeFactor;
}

const QString &Tray::getName() const {
    return name;
}

void Tray::setName(const QString &name) {
    Tray::name = name;
}

double Tray::getSpeed2TimeFactor() const {
    return speed2TimeFactor;
}

void Tray::setSpeed2TimeFactor(double speed2TimeFactor) {
    Tray::speed2TimeFactor = speed2TimeFactor;
}

double Tray::getSpeed3TimeFactor() const {
    return speed3TimeFactor;
}

void Tray::setSpeed3TimeFactor(double speed3TimeFactor) {
    Tray::speed3TimeFactor = speed3TimeFactor;
}

double Tray::getSpeed4TimeFactor() const {
    return speed4TimeFactor;
}

void Tray::setSpeed4TimeFactor(double speed4TimeFactor) {
    Tray::speed4TimeFactor = speed4TimeFactor;
}

double Tray::getSpeed5TimeFactor() const {
    return speed5TimeFactor;
}

void Tray::setSpeed5TimeFactor(double speed5TimeFactor) {
    Tray::speed5TimeFactor = speed5TimeFactor;
}

double Tray::getSpeed6TimeFactor() const {
    return speed6TimeFactor;
}

void Tray::setSpeed6TimeFactor(double speed6TimeFactor) {
    Tray::speed6TimeFactor = speed6TimeFactor;
}

double Tray::motor8SpeedCalFactor1() const
{
    return m_motor8SpeedCalFactor1;
}

void Tray::setMotor8SpeedCalFactor1(double newMotor8SpeedCalFactor1)
{
    m_motor8SpeedCalFactor1 = newMotor8SpeedCalFactor1;
}

double Tray::motor8SpeedCalFactor2() const
{
    return m_motor8SpeedCalFactor2;
}

void Tray::setMotor8SpeedCalFactor2(double newMotor8SpeedCalFactor2)
{
    m_motor8SpeedCalFactor2 = newMotor8SpeedCalFactor2;
}

double Tray::motor8SpeedCalFactor3() const
{
    return m_motor8SpeedCalFactor3;
}

void Tray::setMotor8SpeedCalFactor3(double newMotor8SpeedCalFactor3)
{
    m_motor8SpeedCalFactor3 = newMotor8SpeedCalFactor3;
}

double Tray::motor8SpeedCalFactor4() const
{
    return m_motor8SpeedCalFactor4;
}

void Tray::setMotor8SpeedCalFactor4(double newMotor8SpeedCalFactor4)
{
    m_motor8SpeedCalFactor4 = newMotor8SpeedCalFactor4;
}

double Tray::motor8SpeedCalFactor5() const
{
    return m_motor8SpeedCalFactor5;
}

void Tray::setMotor8SpeedCalFactor5(double newMotor8SpeedCalFactor5)
{
    m_motor8SpeedCalFactor5 = newMotor8SpeedCalFactor5;
}

double Tray::motor8SpeedCalFactor6() const
{
    return m_motor8SpeedCalFactor6;
}

void Tray::setMotor8SpeedCalFactor6(double newMotor8SpeedCalFactor6)
{
    m_motor8SpeedCalFactor6 = newMotor8SpeedCalFactor6;
}
