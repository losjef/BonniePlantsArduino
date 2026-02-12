#ifndef MODBUSOUTPUT_H
#define MODBUSOUTPUT_H

#include <QObject>
#include <QMutex>

class ModbusOutput : public QObject
{
 Q_OBJECT

 public:
  explicit ModbusOutput(QObject *parent = nullptr);
  ~ModbusOutput();
  quint16 getOutputStatus() const;
  void setOutputStatus(quint16 mOutputStatus);
  int getOutputAddress() const;
  void setOutputAddress(int mOutputAddress);
  int getModbusDeviceId() const;
  void setModbusDeviceId(int mModbusDeviceId);

 private:
  QMutex m_mutex;
  quint16 m_outputStatus{0};
  quint16 m_cachedStatus{false};
  int m_outputAddress{0};
  int m_modbusDeviceID{0};

 signals:
  void outputStatusChanged();

};

#endif // MODBUSOUTPUT_H
