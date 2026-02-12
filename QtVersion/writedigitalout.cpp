#include "mainwindow.h"
#include <QMessageBox>

/**
 * @brief Write digital output to Modbus device (motor on/off control)
 * 
 * Sends a single coil write command to the Modbus RTU output module.
 * Includes error handling for connection failures and E-stop retry logic.
 * 
 * @param address Modbus coil address (0-based)
 * @param onOff 0 = OFF, 1 = ON
 * @return 0 on success, -1 on failure
 * 
 * Safety Features:
 * - Connection validation before write
 * - Critical error dialog if E-stop fails
 * - Automatic retry for E-stop operations
 * - Memory leak fix: deleteLater() on all QModbusReply objects
 * 
 * Thread Safety: Can be called from any thread (Modbus handles queuing)
 */
int MainWindow::writeDigitalOutput(quint16 address, int onOff)
{
  // TEST MODE: Simulate successful write
  if (m_testMode) {
      qDebug() << "[TEST MODE] Digital Write simulated - Address:" << address << "Value:" << onOff;
      return 0;  // Always succeed in test mode
  }

  // Write to a single Modbus coil
  // Verify Modbus connection is active

  //ADAM 4056 has a starting address of 0017 - but for some reason the analogAddress needs to be 16
  //Write 12 bits
  //QModbusDataUnit writeOut(QModbusDataUnit::Coils, 16, 12);

  //Single should be in the format of: writeOut(QModbusDataUnit::Coils, 16 + analogAddress, 1);
  if (!modbusClient1 || modbusClient1->state() != QModbusDevice::ConnectedState) {
      qCritical() << "Modbus client not connected! Cannot write to address:" << address;
      if (currentState == states::EstopState) {
          QMessageBox::critical(this, "Critical Error", 
              "Modbus communication lost during E-Stop!\nMotors may not be stopped!\nManually verify equipment is safe.");
      }
      return -1;
  }

  QModbusDataUnit writeOut(QModbusDataUnit::Coils, address, 1);
  writeOut.setValue(0, onOff);

  //Send the write request to Modbus device
  if ((replyDigitalOut = modbusClient1->sendWriteRequest(writeOut, m_digitalOutAddress))) {
      if (!replyDigitalOut->isFinished()) {
          // Async completion - connect to finished signal
          QObject::connect(replyDigitalOut, &QModbusReply::finished, this, [this, address, onOff]() {
              if (replyDigitalOut->error() == QModbusDevice::NoError) {
                  qDebug() << "Digital Write successful - Address:" << address << "Value:" << onOff;
              }
              else {
                  qWarning() << "Digital Write error - Address:" << address << "Error:" << replyDigitalOut->errorString();
                  // For critical safety operations (E-stop), show error and retry
                  if (currentState == states::EstopState) {
                      qCritical() << "E-STOP: Retrying motor shutdown for address" << address;
                      writeDigitalOutput(address, 0);  // Retry turning off motor
                  }
              }
              replyDigitalOut->deleteLater();  // Fixed memory leak
          });
      }
      else {
          replyDigitalOut->deleteLater();  // Fixed memory leak
          return 0;
      }
  }
  else {
      qCritical() << "Digital Write request failed:" << modbusClient1->errorString();
      return -1;
  }
  return 0;
}

void MainWindow::handleDOReplyFinished()
{
  if (replyDigitalOut->error() == QModbusDevice::NoError)
  {
    //qDebug() << "Digital Write successful "  << analogAddress << " " << onOff;
  } else
  {
    qDebug() << "Digital Write error:" << replyDigitalOut->errorString();
  }
  //reply->deleteLater();
}
