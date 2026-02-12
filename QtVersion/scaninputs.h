#ifndef SCANINPUTS_H
#define SCANINPUTS_H

#include <QObject>
#include <QTimer>
#include <QtSerialBus>
#include <QModbusClient>
#include <QModbusRtuSerialClient>

class ScanInputs : public QObject
{
    Q_OBJECT
public:
    explicit ScanInputs();
    //explicit ScanInputs(QSharedPointer<QModbusRtuSerialClient> serialClient,  int analogAddress);
    ~ScanInputs();

    void stop();
    void connectModbus(QString port);
    void setComPort(QString port);

public slots:
    void run();

private:
    QSharedPointer<QModbusRtuSerialClient> modbusClient;
    QTimer *m_timer;
    QModbusDataUnit request;
    QList<bool> inputCache;

    bool connected {false};
    int m_address {2};
    const int m_inputCount {8};
    QString m_portName{ "COM5" };
    int m_timerDelay{ 100 };

    void onReplyFinished();
    void timeout();

signals:
    void inputChanged(int address, bool value);
    //void modbusConnected(bool connected);

};

#endif // SCANINPUTS_H
