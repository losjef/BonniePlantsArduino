#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>

class Motor : public QObject
{
 Q_OBJECT

 public:
  explicit Motor(QObject *parent = nullptr);
  explicit Motor(QList<double> speedCalFactors);

  double speedCalFactor1() const;
  void setSpeedCalFactor1(double newSpeedCalFactor1);

  double speedCalFactor2() const;
  void setSpeedCalFactor2(double newSpeedCalFactor2);

  double speedCalFactor3() const;
  void setSpeedCalFactor3(double newSpeedCalFactor3);

  double speedCalFactor4() const;
  void setSpeedCalFactor4(double newSpeedCalFactor4);

  double speedCalFactor5() const;
  void setSpeedCalFactor5(double newSpeedCalFactor5);

  double speedCalFactor6() const;
  void setSpeedCalFactor6(double newSpeedCalFactor6);

  void setSpeedCalFactors(QList<double> speedCalFactors);

  QString name() const;
  void setName(const QString &newName);

  double speed() const;
  void setSpeed(double newSpeed);

  int analogAddress() const;
  void setAnalogAddress(int newAddress);

  int digitalOutAddress() const;
  void setDigitalAddress(int m_digital_address);

  int modbusDigitalDeviceID() const;
  void setModbusDigitalDeviceID(int newModbusDeviceID);

  int modbusAnalogDeviceID() const;
  void setModbusAnalogDeviceID(int newModbusAnalogDeviceID);

  private:
  QString m_name;

  double m_speed{0};
  int m_analogAddress{0};
  int m_digitalAddress{0};
  int m_modbusDigitalDeviceID{0};
  int m_modbusAnalogDeviceID{0};

  // Speed factors for motors 1 thru 6
  double m_speedCalFactor1 = 1.0;
  double m_speedCalFactor2 = 1.0;
  double m_speedCalFactor3 = 1.0;
  double m_speedCalFactor4 = 1.0;
  double m_speedCalFactor5 = 1.0;
  double m_speedCalFactor6 = 1.0;

 signals:

  void speedCalFactor1Changed();
  void speedCalFactor2Changed();
  void speedCalFactor3Changed();
  void speedCalFactor4Changed();
  void speedCalFactor5Changed();
  void speedCalFactor6Changed();
  void nameChanged();
  //void speedChanged();
};

#endif // MOTOR_H
