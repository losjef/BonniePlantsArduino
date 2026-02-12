//
// Created by jeffl on 3/5/2024.
//
#include <QMessageBox>
#include "ReadWriteModbus.h"

ReadWriteModbus::ReadWriteModbus(QObject *parent)
{
    QThread::currentThread()->setObjectName("Modbus Thread");
}

ReadWriteModbus::~ReadWriteModbus()
{
    qInfo() << "Modbus Destuctor";
    if (connected)
    {
        modbusClient->disconnectDevice();
    }
}

void ReadWriteModbus::run()
{
    inputCache.resize(m_inputCount);
    inputCache.fill(false);

    //start the timer
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ReadWriteModbus::timeout);
    m_timer->setInterval(readWriteDelay);
    //if (connected)
    {
        m_timer->start();
    }
}

void ReadWriteModbus::timeout()
{

    // if (outputsHaveChanged)
    // {
    //      //writeAllDigitalOutputs();
    //     outputsHaveChanged = false;
    // }
    // if (analogOutputsHaveChanged)
    // {
    //     //		writeAnalogOutput(motors["Infeed Belt"]->analogAddress(), motors["Infeed Belt"]->speed());
    //     //		writeAnalogOutput(motors["Lower Soil Belt"]->analogAddress(), motors["Lower Soil Belt"]->speed());
    //     //		writeAnalogOutput(motors["Flat Filler Belt"]->analogAddress(), motors["Flat Filler Belt"]->speed());
    //     //		writeAnalogOutput(motors["Planting Line Belt"]->analogAddress(), motors["Planting Line Belt"]->speed());
    //     //		writeAnalogOutput(motors["Motor5"]->analogAddress(), motors["Motor5"]->speed());
    //     //		writeAnalogOutput(motors["Motor6"]->analogAddress(), motors["Motor6"]->speed());
    //     //writeAnalogOutput(motor8->getAnalogAddress(), motor8->getAnalogValue());
    //     //writeAnalogOutput(motors["Motor8"]->analogAddress(), motors["Motor8"]->speed());  //Motor8 is not part of the hash
    //     analogOutputsHaveChanged = false;
    // }

    //Disable for testing
    readDigitalInputs();

}

void ReadWriteModbus::stop()
{
    OktoRun = false;
}

void ReadWriteModbus::connectModbus()
{
    //enable modbus logging
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));

    modbusClient = QSharedPointer<QModbusRtuSerialClient>::create();
    modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_portName);
    modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
    modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud57600);
    modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
    modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);

    if (!modbusClient->connectDevice())
    {
        connected = false;
        qWarning() << "Error Connecting to Modbus Device";

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Output Modbus not connected");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    else
        connected = true;
}

void ReadWriteModbus::readDigitalInputs()
{
    //qInfo() << "ScanInputs timer timeout";
    // Send the read request and get a QModbusReply object
    QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs, 0, 8);
    if (auto* reply = modbusClient->sendReadRequest(readUnit, 1))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, &ReadWriteModbus::handleDIReplyFinished);
        }
        else
        {
            delete reply; // broadcast replies return immediately
        }
    }
    else
    {
        qDebug() << "Read error: " << modbusClient->errorString();
    }
    // QModbusDataUnit inputRequest(QModbusDataUnit::DiscreteInputs, 0, 8);
    // QModbusReply *reply = modbusClient->sendReadRequest(inputRequest, input1Address);

    // // Check if the reply is valid
    // if (reply)
    // {
    //     // Connect a slot to the finished signal of the reply
    //     connect(reply, &QModbusReply::finished, this, &ReadWriteModbus::handleDIReplyFinished);
    // }
    // else
    // {
    //     // Handle the error
    //     qWarning() << "Send error:" << modbusClient->errorString();
    // }
    // reply->deleteLater();
}

void ReadWriteModbus::handleDIReplyFinished()
{
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (qsizetype i = 0, total = unit.valueCount(); i < total; ++i) {
            //const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
            //                          .arg(QString::number(unit.value(i), unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            //qDebug() << entry;
            QModbusDataUnit result = reply->result();
            bool coilStatus = result.value(i); // Get the value as a bool
            if (coilStatus != inputCache[i])
                        {
                            emit inputChanged(i, coilStatus);
                            inputCache[i] = coilStatus;
                            //qInfo() << "Input" << i << "changed to" << coilStatus;
                        }
        }
    }
    else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() << "Read response protocol error";
    }
    else {
        qDebug() << "Read response error";
    }

    reply->deleteLater();
    // QModbusReply *reply = qobject_cast<QModbusReply *>(sender());
    // if (!reply)
    // {
    //     return;
    // }

    // // Check if the reply is finished and has no error
    // if (reply->isFinished() && reply->error() == QModbusDevice::NoError)
    // {
    //     // Get the result data unit from the reply
    //     QModbusDataUnit result = reply->result();

    //     // Loop through the values and print the coil status
    //     for (int i = 0; i < result.valueCount(); i++)
    //     {
    //         bool coilStatus = result.value(i); // Get the value as a bool
    //         qDebug() << "Coil" << i << "status:" << coilStatus;

    //         //TODO Need to test
    //         if (coilStatus != inputCache[i])
    //         {
    //             emit inputChanged(i, coilStatus);
    //             inputCache[i] = coilStatus;
    //             //qInfo() << "Input" << i << "changed to" << coilStatus;
    //         }
    //     }
    // }
    // else
    // {
    //     // Handle the error
    //     qDebug() << "Read error:" << reply->errorString();
    // }

    // // Delete the reply object
    // reply->deleteLater();
}

//void ReadWriteModbus::writeAllDigitalOutputs()
//{
//    //TODO: currently writing deviceID 1 only - 2 needs to be written
//    // Write all coils
//    QModbusDataUnit writeOut(QModbusDataUnit::Coils, 0, 8);
//    // writeOut.setRegisterType(QModbusDataUnit::Coils);
//    // writeOut.setStartAddress(0);
//    // writeOut.setValueCount(8);
//
//    writeOut.setValue(0, m_outputList1[0]);
//    writeOut.setValue(1, m_outputList1[1]);
//    writeOut.setValue(2, m_outputList1[2]);
//    writeOut.setValue(3, m_outputList1[3]);
//    writeOut.setValue(4, m_outputList1[4]);
//    writeOut.setValue(5, m_outputList1[5]);
//    writeOut.setValue(6, m_outputList1[6]);
//    writeOut.setValue(7, m_outputList1[7]);
//
//    //	writeOut.setValue(motor1DigitalOut.getOutputAddress(), motor1DigitalOut.getOutputStatus());
//    //	writeOut.setValue(motor2DigitalOut.getOutputAddress(), motor2DigitalOut.getOutputStatus());
//    //	writeOut.setValue(motor3DigitalOut.getOutputAddress(), motor3DigitalOut.getOutputStatus());
//    //	writeOut.setValue(motor4DigitalOut.getOutputAddress(), motor4DigitalOut.getOutputStatus());
//    //	writeOut.setValue(motor5DigitalOut.getOutputAddress(), motor5DigitalOut.getOutputStatus());
//    //	writeOut.setValue(motor6DigitalOut.getOutputAddress(), motor6DigitalOut.getOutputStatus());
//    //writeOut.setValue(motor8DigitalOut.getOutputAddress(), motor8DigitalOut.getOutputStatus());
//    //writeOut.setValue(redLightDigitalOut.getOutputAddress(), redLightDigitalOut.getOutputStatus());
//    //writeOut.setValue(yellowLightDigitalOut.getOutputAddress(), yellowLightDigitalOut.getOutputStatus());
//    //writeOut.setValue(greenLightDigitalOut.getOutputAddress(), greenLightDigitalOut.getOutputStatus());
//    //writeOut.setValue(buzzerDigitalOut.getOutputAddress(), buzzerDigitalOut.getOutputStatus());
//
//    //Send the write request to Modbus device
//    //TODO: Need to handle when the output modubus device address is different - looks like there will be #1 and #2
//    if (auto *replyDigitalOut = modbusClient->sendWriteRequest(writeOut, 1))
//    {
//        if (!replyDigitalOut->isFinished())
//        {
//            connect(replyDigitalOut, &QModbusReply::finished, this, &ReadWriteModbus::handleDOReplyFinished);
//        }
//        else
//        {
//            //replyDigitalOut->deleteLater();
//            //delete replyDigitalOut;
//            return;
//        }
//    }
//    else
//    {
//        //replyDigitalOut->deleteLater();
//        //delete replyDigitalOut;
//        qDebug() << "Digital Write error:" << modbusClient->errorString();
//        return;
//    }
//}

void ReadWriteModbus::writeDigitalOutput(quint16 address, quint16 onOff, quint16 deviceID)
{
    QModbusDataUnit writeOut(QModbusDataUnit::Coils, address, 1);  //Write a single coil
    writeOut.setValue(0, onOff);

    //Send the write request to Modbus device
    if (auto *replyDigitalOut = modbusClient->sendWriteRequest(writeOut, deviceID))
    {
        if (!replyDigitalOut->isFinished())
        {
            connect(replyDigitalOut, &QModbusReply::finished, this, &ReadWriteModbus::handleDOReplyFinished);
        }
        else
        {
            //replyDigitalOut->deleteLater();
            //delete replyDigitalOut;
            return;
        }
    }
    else
    {
        //replyDigitalOut->deleteLater();
        //delete replyDigitalOut;
        qDebug() << "Digital Write error:" << modbusClient->errorString();
        return;
    }
}

void ReadWriteModbus::handleDOReplyFinished()
{
    auto replyDigitalOut = qobject_cast<QModbusReply *>(sender());
    if (replyDigitalOut->error() == QModbusDevice::NoError)
    {
        qDebug() << "Digital Write successful";
        //delete replyDigitalOut;
        return;
    }
    else
    {
        qDebug() << "Digital Write error:" << replyDigitalOut->errorString();
        //delete replyDigitalOut;
    }
}

int ReadWriteModbus::writeAnalogOutput(int motorAddress, int percent)
{
    int analogValue{0};
    if (percent > 100) percent = 100;
    analogValue = calculateAnalogValue(percent);

    QModbusDataUnit writeAnalogOut(QModbusDataUnit::HoldingRegisters, motorAddress, 1);
    writeAnalogOut.setValue(0, analogValue);

    //Send the write request to Modbus device analogAddress 2
    if (auto *replyAnalogOut = modbusClient->sendWriteRequest(writeAnalogOut, 3))
    {
        if (!replyAnalogOut->isFinished())
        {
            QObject::connect(replyAnalogOut, &QModbusReply::finished, this, &ReadWriteModbus::handleAOReplyFinished);
        }
        else
        {
            //replyAnalogOut->deleteLater();
            delete replyAnalogOut;
            return 0;
        }
    }
    else
    {
        qDebug() << "Analog Write error:" << modbusClient->errorString();
        //replyAnalogOut->deleteLater();
        delete replyAnalogOut;
        return -1;
    }
    return 0;
}

void ReadWriteModbus::handleAOReplyFinished()
{
    auto replyAnalogOut = qobject_cast<QModbusReply *>(sender());
    if (replyAnalogOut->error() == QModbusDevice::NoError)
    {
        qDebug() << "Digital Write successful";
        return;
    }
    else
    {
        qDebug() << "Digital Write error:" << replyAnalogOut->errorString();
    }
}

int ReadWriteModbus::calculateAnalogValue(int percent)
{
    // percent is 0 to 100
    // output voltage = 10VDC * percent
    // output value = ((outputVoltage + 10)* 4095)/20

    double voltage{0};
    voltage = 10.0 * percent / 100.0;
    //return (int)((voltage + 10.0) * 4095.0) / 20.0;
    return static_cast<int>((voltage + 10.0) * 4095.0 / 20.0);
}

void ReadWriteModbus::writeDO(quint16 address, quint16 onOff, quint16 deviceID)
{
    writeDigitalOutput(address, onOff, deviceID);
}

void ReadWriteModbus::digitalOutputChanged(QList<quint16> outputList, int deviceID)
{
    m_timer->stop();
   if(deviceID == 1)
   {
       //Can also track with deviceID changed if necessary - would need to outputsHaveChanged Variables
       m_outputList1[0] = outputList[0];
       writeDigitalOutput(0, m_outputList1[0], 1);
       m_outputList1[1] = outputList[1];
       writeDigitalOutput(1, m_outputList1[1], 1);
       m_outputList1[2] = outputList[2];
       writeDigitalOutput(2, m_outputList1[2], 1);
       m_outputList1[3] = outputList[3];
       writeDigitalOutput(3, m_outputList1[3], 1);
       m_outputList1[4] = outputList[4];
       writeDigitalOutput(4, m_outputList1[4], 1);
       m_outputList1[5] = outputList[5];
       writeDigitalOutput(5, m_outputList1[5], 1);
       m_outputList1[6] = outputList[6];
       writeDigitalOutput(6, m_outputList1[6], 1);
       m_outputList1[7] = outputList[7];
       writeDigitalOutput(7, m_outputList1[7], 1);
   }
   else if(deviceID == 2)
   {
       m_outputList2[0] = outputList[0];
       m_outputList2[1] = outputList[1];
       m_outputList2[2] = outputList[2];
       m_outputList2[3] = outputList[3];
       m_outputList2[4] = outputList[4];
       m_outputList2[5] = outputList[5];
       m_outputList2[6] = outputList[6];
       m_outputList2[7] = outputList[7];
   }
   else
   {
       qWarning() << "Invalid deviceID";
   }
   outputsHaveChanged = true;
   m_timer->start();
}
