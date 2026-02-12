#include "mainwindow.h"

//Type Codes - in HEX
//30 = 0 to 20 mA
//31 = 4 to 20 mA
//32 = 0 to 10 V

//ADDR 4X Channel Item Attribute Memo
//40001 0 Current Output Value R / W
//40002 1 Current Output Value R / W
//40003 2 Current Output Value R / W
//40004 3 Current Output Value R / W
// 
//40201 0 Type Code R / W
//40202 1 Type Code R / W
//40203 2 Type Code R / W
//40204 3 Type Code R / W

int MainWindow::writeAnalogOutput(int motorAddress, int percent)
{
    // TEST MODE: Simulate successful write
    if (m_testMode) {
        int voltage_mV = (percent * 10000) / 100;
        qDebug() << "[TEST MODE] Analog Write simulated - Motor:" << motorAddress 
                 << "Speed:" << percent << "%" 
                 << "Voltage:" << voltage_mV << "mV (" << (voltage_mV/1000.0) << "V)";
        return 0;  // Always succeed in test mode
    }

    // Verify Modbus connection is active
    if (!modbusClient1 || modbusClient1->state() != QModbusDevice::ConnectedState) {
        qWarning() << "Modbus client not connected! Cannot write analog output to address:" << motorAddress;
        return -1;
    }

    int analogValue {0};
    if(percent > 100) percent = 100;
    if(percent < 0) percent = 0;  // Safety: clamp to valid range
    analogValue = calculateAnalogValue(percent);

    QModbusDataUnit writeAnalogOut(QModbusDataUnit::HoldingRegisters, motorAddress, 1);
    writeAnalogOut.setValue(0, analogValue);

    //Send the write request to Modbus device (Device ID 3 - Waveshare Analog Output 8CH)
    if ((replyAnalogOut = modbusClient1->sendWriteRequest(writeAnalogOut, m_analogOutAddress))) {
        if (!replyAnalogOut->isFinished()) {
            QObject::connect(replyAnalogOut, &QModbusReply::finished, this, [this, motorAddress, percent, analogValue]() {
                if (replyAnalogOut->error() == QModbusDevice::NoError) {
                    qDebug() << "Analog Write successful - Motor:" << motorAddress 
                             << "Speed:" << percent << "%" 
                             << "Value:" << analogValue << "mV (" << (analogValue/1000.0) << "V)";
                }
                else {
                    qWarning() << "Analog Write error - Motor:" << motorAddress << "Error:" << replyAnalogOut->errorString();
                }
                replyAnalogOut->deleteLater();  // Fixed memory leak
            });
        }
        else {
            replyAnalogOut->deleteLater();  // Fixed memory leak
            return 0;
        }
    }
    else {
        qWarning() << "Analog Write request failed:" << modbusClient1->errorString();
        return -1;
    }
    return 0;
}

int MainWindow::calculateAnalogValue(int percent)
{
    // Waveshare Modbus RTU Analog Output 8CH (B) expects millivolts (0-10000 mV)
    // percent: 0-100
    // Output: 0-10000 mV (0-10V)
    // Formula: percent * 100 = millivolts
    // Examples: 0% = 0 mV, 50% = 5000 mV (5V), 100% = 10000 mV (10V)
    // Note: This formula has been tested and verified with actual hardware
    
    return percent * 100;  // Verified correct: 50% = 5000 mV = 5V
}

//int MainWindow::initializeAnalogOutput()
//{
//    QModbusDataUnit writeInitialAnalogOut(QModbusDataUnit::HoldingRegisters, 200, 4);
//    writeInitialAnalogOut.setValue(0, 48); //set the type code to 0 to 10 V *** 48 is the wrong value
//    writeInitialAnalogOut.setValue(1, 48); //set the type code to 0 to 10 V
//    writeInitialAnalogOut.setValue(2, 48); //set the type code to 0 to 10 V
//    writeInitialAnalogOut.setValue(3, 48); //set the type code to 0 to 10 V
//    //Send the write request to Modbus device
//    if ((replyAnalogInitial = modbusClient1->sendWriteRequest(writeInitialAnalogOut, m_analogOutAddress))) {
//        if (!replyAnalogInitial->isFinished()) {
//            QObject::connect(replyAnalogInitial, &QModbusReply::finished, [&]() {
//                if (replyAnalogInitial->error() == QModbusDevice::NoError) {
//                    qDebug() << "Write Initialization successful";
//                }
//                else {
//                    qDebug() << "Write Analog Initialization error:" << replyAnalogInitial->errorString();
//                }
//                //replyAnalogInitial->deleteLater();
//                return 0;
//            });
//        }
//        else {
//            //delete replyAnalogInitial;
//            return 0;
//        }
//    }
//    else {
//        qDebug() << "Write Initialization error:" << modbusClient1->errorString();
//        return -1;
//    }
//    return 0;
//}
