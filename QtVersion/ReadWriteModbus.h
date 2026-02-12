//
// Created by jeffl on 3/5/2024.
//

#ifndef CONVEYORINTERFACEQT_READWRITEMODBUS_H_
#define CONVEYORINTERFACEQT_READWRITEMODBUS_H_

#include <QObject>
#include <QTimer>
#include <QtSerialBus>
//#include <QModbusClient>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QMutex>

class ReadWriteModbus : public QObject
{
 Q_OBJECT

 public:
  explicit ReadWriteModbus(QObject *parent = nullptr);
  ~ReadWriteModbus();

  //void createMembers(QHash<QString, QSharedPointer<Motor>> motors);
  void stop();
  void connectModbus();

 public slots:
  void run();
  void digitalOutputChanged(QList<quint16> outputList, int deviceID);
  void writeDO(quint16 address, quint16 onOff, quint16 deviceID);


 private:
  QTimer *m_timer;
  const int readWriteDelay{100};
  bool OktoRun{true};
  QList<quint16> m_outputList1 {0, 0, 0, 0, 0, 0, 0, 0};
  QList<quint16> m_outputList2 {0, 0, 0, 0, 0, 0, 0, 0};
  QMutex mutex;

  //Mobus Data
  const QString m_portName{"COM3"};
  bool connected{false};

  //Inputs
  QModbusDataUnit inputRequest;
  int input1Address{1};
  QList<bool> inputCache;
  const int m_inputCount{8};
  //Set input addresses
  const int startButtonAddress{0};
  //ModbusInput startButton;
  const int stopButtonAddress{1};
  //ModbusInput stopButton;
  const int startDelayButtonAddress{2};
  //ModbusInput startDelayButton;
  const int eStopButtonAddress{3};
  //ModbusInput eStopButton;

  //Outputs
  volatile bool outputsHaveChanged{false};
  volatile bool analogOutputsHaveChanged{false};
  //Set output addresses

  QSharedPointer<QModbusRtuSerialClient> modbusClient;

  //QModbusDataUnit writeOut;

  //void writeAllDigitalOutputs();
  void writeDigitalOutput(quint16 address, quint16 value, quint16 deviceID);
  void handleDOReplyFinished();
  int writeAnalogOutput(int percent, int channel);
  void handleAOReplyFinished();
  int calculateAnalogValue(int percent);
  void readDigitalInputs();
  void handleDIReplyFinished();
  void timeout();

  //QModbusReply *replyDigitalOut;
  //QModbusReply *replyAnalogOut;
  //QModbusReply *replyAnalogInitial;

 signals:
  void inputChanged(int address, bool value);
};

#endif //CONVEYORINTERFACEQT_READWRITEMODBUS_H_
