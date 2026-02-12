#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "motorfactors.h"
#include "traycalibfactors.h"
#include "updatecomports.h"
#include "uppersoilbeltfact.h"
#include "logviewer.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QStatusBar>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButtonSaveTimer->setEnabled(false);
    QThread::currentThread()->setObjectName("Main Thread");

    // Check for test mode: compile-time flag OR runtime environment variable
#ifdef CONVEYOR_TEST_MODE
    m_testMode = true;
    qWarning() << "***** TEST MODE ENABLED (compile-time) *****";
#else
    m_testMode = qEnvironmentVariableIntValue("CONVEYOR_TEST_MODE") == 1;
    if (m_testMode) {
        qWarning() << "***** TEST MODE ENABLED (environment variable) *****";
    }
#endif

    if (m_testMode) {
        qWarning() << "Modbus communication will be simulated";
        setWindowTitle(windowTitle() + " [TEST MODE]");
        statusBar()->showMessage("⚠️ TEST MODE - Simulated Hardware", 0);
        statusBar()->setStyleSheet("QStatusBar { background-color: yellow; color: black; font-weight: bold; }");
    }

    //enable modbus logging
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));

    createMembers();

    ui->labelTimer->setText(QString::number(waitTime));

    if (!m_testMode) {
        createQModbusRtuSerialClient();
        CreateInputScanThread();
    } else {
        // Test mode: Create mock Modbus client (always connected)
        modbusClient1 = QSharedPointer<QModbusRtuSerialClient>::create();
        qInfo() << "Test mode: Modbus client mocked (no hardware required)";
        // Skip input scan thread in test mode (we'll use UI buttons instead)
    }

    if (m_testMode) {
        setupTestModeUI();
    }

    // Update timer display every second
    connect(&timerMotors, &QTimer::timeout, this, &MainWindow::countDownTimerDecrement);

    // TIMER-BASED PLANT COUNTING: Each timer tick = one plant counted
    // Timer interval calculated from belt speed + tray spacing + speed setting
    // This provides consistent counting without physical sensors per plant
    connect(&timerCounter, &QTimer::timeout, this, &MainWindow::incrementCurrentCounter);

    //connect input scan signal to input changed slot
    connect(&myInputScan, &ScanInputs::inputChanged, this, &MainWindow::inputChanged);

    //connect buzzer timer to buzzer function
    connect(&timerBuzzer, &QTimer::timeout, this, &MainWindow::buzzerTimerDecrement);

    turn_all_outputs_off();
    stateStop();
}

void MainWindow::createMembers()
{
    motor1 = QSharedPointer<Motor>(new Motor);
    motor1->setAnalogAddress(m_motor1AnalogOutAddress);
    motor1->setDigitalAddress(m_motor1DigitalOutAddress);
    motor1->setModbusDigitalDeviceID(m_motor1ModbusDigitalDeviceID);
    motor1->setModbusAnalogDeviceID(m_motor1ModbusAnalogDeviceID);
    motor1->setName(m_motor1Name);

    motor2 = QSharedPointer<Motor>(new Motor);
    motor2->setAnalogAddress(m_motor2AnalogOutAddress);
    motor2->setDigitalAddress(m_motor2DigitalOutAddress);
    motor2->setModbusDigitalDeviceID(m_motor2ModbusDigitalDeviceID);
    motor2->setModbusAnalogDeviceID(m_motor2ModbusAnalogDeviceID);
    motor2->setName(m_motor2Name);

    motor3 = QSharedPointer<Motor>(new Motor);
    motor3->setAnalogAddress(m_motor3AnalogOutAddress);
    motor3->setDigitalAddress(m_motor3DigitalOutAddress);
    motor3->setModbusDigitalDeviceID(m_motor3ModbusDigitalDeviceID);
    motor3->setModbusAnalogDeviceID(m_motor3ModbusAnalogDeviceID);
    motor3->setName(m_motor3Name);

    motor4 = QSharedPointer<Motor>(new Motor);
    motor4->setAnalogAddress(m_motor4AnalogOutAddress);
    motor4->setDigitalAddress(m_motor4DigitalOutAddress);
    motor4->setModbusDigitalDeviceID(m_motor4ModbusDigitalDeviceID);
    motor4->setModbusAnalogDeviceID(m_motor4ModbusAnalogDeviceID);
    motor4->setName(m_motor4Name);

    motor5 = QSharedPointer<Motor>(new Motor);
    motor5->setAnalogAddress(m_motor5AnalogOutAddress);
    motor5->setDigitalAddress(m_motor5DigitalOutAddress);
    motor5->setModbusDigitalDeviceID(m_motor5ModbusDigitalDeviceID);
    motor5->setModbusAnalogDeviceID(m_motor5ModbusAnalogDeviceID);
    motor5->setName(m_motor5Name);

    motor6 = QSharedPointer<Motor>(new Motor);
    motor6->setAnalogAddress(m_motor6AnalogOutAddress);
    motor6->setDigitalAddress(m_motor6DigitalOutAddress);
    motor6->setModbusDigitalDeviceID(m_motor6ModbusDigitalDeviceID);
    motor6->setModbusAnalogDeviceID(m_motor6ModbusAnalogDeviceID);
    motor6->setName(m_motor6Name);

    //motor7 = QSharedPointer<Motor>(new Motor);
    //motor7->setAnalogAddress(m_motor7DigitalOutAddress);

    motor8 = QSharedPointer<Motor>(new Motor);
    motor8->setAnalogAddress(m_motor8AnalogOutAddress);
    motor8->setDigitalAddress(m_motor8DigitalOutAddress);
    motor8->setModbusDigitalDeviceID(m_motor8ModbusDigitalDeviceID);
    motor8->setModbusAnalogDeviceID(m_motor8ModbusAnalogDeviceID);
    motor8->setName(m_motor8Name);

    tray1 = QSharedPointer<Tray>(new Tray);
    tray2 = QSharedPointer<Tray>(new Tray);
    tray3 = QSharedPointer<Tray>(new Tray);
    tray4 = QSharedPointer<Tray>(new Tray);
    tray5 = QSharedPointer<Tray>(new Tray);
    tray6 = QSharedPointer<Tray>(new Tray);

    totalCounter.readTotalCounter();
    m_totalCounter = totalCounter.count();
    ui->lcdTotalCounter->display(m_totalCounter);

    readMotorJson();
    readTrayJson();
    readTimerJson();
    readCOMPorts();
    // ui->pushButtonTray1->clicked(true);
    // emit on_pushButtonTray1_clicked();
    currentTray = tray1;
    readUpperSoilBeltJson(currentTray);  // I think we need to populate motor8 with default values or this crashes - actual tray must be selected before start

}

MainWindow::~MainWindow()
{
    qInfo() << "MainWindow Destructor";
    modbusClient1->disconnectDevice();
    inputScanThread.terminate();
    inputScanThread.wait();
    delete ui;
}

void MainWindow::CreateInputScanThread()
{
    inputScanThread.setObjectName("Input Scan Thread");
    //inputScanThread.setParent(this);

    myInputScan.moveToThread(&inputScanThread);
    myInputScan.setComPort(m_inputPortName);

    connect(&inputScanThread, &QThread::started, &myInputScan, &ScanInputs::run);

    inputScanThread.start();
}

int MainWindow::createQModbusRtuSerialClient() {
    if (m_testMode) {
        qInfo() << "Test mode: Skipping actual Modbus connection";
        return 0;  // Simulate successful connection
    }

    modbusClient1 = QSharedPointer<QModbusRtuSerialClient>::create();
    //set up modbus client
    modbusClient1->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_outputPortName);
    modbusClient1->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
    modbusClient1->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud57600);
    modbusClient1->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
    modbusClient1->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);

    // Connect to the Modbus device
    if (!modbusClient1->connectDevice()) {
        qDebug() << "Failed to connect to Modbus Output Device";
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Output Modbus not connected");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return -1;
    }
    return 0;
}

void MainWindow::modbusConnected(bool connected)
{
    if (connected) {
        qDebug() << "Modbus connected";
    } else {
        //qWarning() << "Modbus not connected";
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Modbus not connected");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::on_pushButtonSpeed1_clicked()
{
    ui->frameDisplay->setStyleSheet("background-color: red;");
    speedSelected = 1;
    //Update counter interval based on speed selected
    if (currentTray != nullptr)
    {
        counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
        if (currentState == states::Run2State)
            timerCounter.start(static_cast<int>(counterTimerInterval * 1000.0));
        setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    }
    else {
        qWarning() << "Cannot change speed: No tray selected";
    }

}

void MainWindow::on_pushButtonSpeed2_clicked()
{
    handleSpeedChange(2, "orange");
}

void MainWindow::on_pushButtonSpeed3_clicked()
{
    handleSpeedChange(3, "yellow");
}

void MainWindow::on_pushButtonSpeed4_clicked()
{
    handleSpeedChange(1, "red");
}

void MainWindow::on_pushButtonSpeed5_clicked()
{
    handleSpeedChange(5, "blue");
}

void MainWindow::on_pushButtonSpeed6_clicked()
{
    handleSpeedChange(6, "indigo");
}

void MainWindow::setMotorSpeeds(double baseSpeed, int motorCalFactor, const QSharedPointer<Tray>& currentTray)
{
    switch (motorCalFactor)
    {
    case 1:
        motor1->setSpeed(baseSpeed * motor1->speedCalFactor1());
        motor2->setSpeed(baseSpeed * motor2->speedCalFactor1());
        motor3->setSpeed(baseSpeed * motor3->speedCalFactor1());
        motor4->setSpeed(baseSpeed * motor4->speedCalFactor1());
        motor5->setSpeed(baseSpeed * motor5->speedCalFactor1());
        motor6->setSpeed(baseSpeed * motor6->speedCalFactor1());
        motor8->setSpeed(baseSpeed * currentTray->motor8SpeedCalFactor1());
        break;
    case 2:
        motor1->setSpeed(baseSpeed * motor1->speedCalFactor2());
        motor2->setSpeed(baseSpeed * motor2->speedCalFactor2());
        motor3->setSpeed(baseSpeed * motor3->speedCalFactor2());
        motor4->setSpeed(baseSpeed * motor4->speedCalFactor2());
        motor5->setSpeed(baseSpeed * motor5->speedCalFactor2());
        motor6->setSpeed(baseSpeed * motor6->speedCalFactor2());
        motor8->setSpeed(baseSpeed * currentTray->motor8SpeedCalFactor2());
        break;
    case 3:
        motor1->setSpeed(baseSpeed * motor1->speedCalFactor3());
        motor2->setSpeed(baseSpeed * motor2->speedCalFactor3());
        motor3->setSpeed(baseSpeed * motor3->speedCalFactor3());
        motor4->setSpeed(baseSpeed * motor4->speedCalFactor3());
        motor5->setSpeed(baseSpeed * motor5->speedCalFactor3());
        motor6->setSpeed(baseSpeed * motor6->speedCalFactor3());
        motor8->setSpeed(baseSpeed * currentTray->motor8SpeedCalFactor3());
        break;
    case 4:
        motor1->setSpeed(baseSpeed * motor1->speedCalFactor4());
        motor2->setSpeed(baseSpeed * motor2->speedCalFactor4());
        motor3->setSpeed(baseSpeed * motor3->speedCalFactor4());
        motor4->setSpeed(baseSpeed * motor4->speedCalFactor4());
        motor5->setSpeed(baseSpeed * motor5->speedCalFactor4());
        motor6->setSpeed(baseSpeed * motor6->speedCalFactor4());
        motor8->setSpeed(baseSpeed * currentTray->motor8SpeedCalFactor4());
        break;
    case 5:
        motor1->setSpeed(baseSpeed * motor1->speedCalFactor5());
        motor2->setSpeed(baseSpeed * motor2->speedCalFactor5());
        motor3->setSpeed(baseSpeed * motor3->speedCalFactor5());
        motor4->setSpeed(baseSpeed * motor4->speedCalFactor5());
        motor5->setSpeed(baseSpeed * motor5->speedCalFactor5());
        motor6->setSpeed(baseSpeed * motor6->speedCalFactor5());
        motor8->setSpeed(baseSpeed * currentTray->motor8SpeedCalFactor5());
        break;
    case 6:
        motor1->setSpeed(baseSpeed * motor1->speedCalFactor6());
        motor2->setSpeed(baseSpeed * motor2->speedCalFactor6());
        motor3->setSpeed(baseSpeed * motor3->speedCalFactor6());
        motor4->setSpeed(baseSpeed * motor4->speedCalFactor6());
        motor5->setSpeed(baseSpeed * motor5->speedCalFactor6());
        motor6->setSpeed(baseSpeed * motor6->speedCalFactor6());
        motor8->setSpeed(baseSpeed * currentTray->motor8SpeedCalFactor6());
        break;
    default:break;
    }

    sendMotorSpeedsToModbus();
}

void MainWindow::on_pushButtonStop_clicked()
{
    //Enter Stop state from any state except EStopState
    if(currentState != states::EstopState)
        stateStop();
}

void MainWindow::on_pushButtonStart_clicked()
{
    //Recognize start pressed in StopState only
    if (currentState == states::StopState)
    {
        QString warningMessage;
        if (speedSelected == 0 && currentTray == nullptr)
        {
            warningMessage = "Please select a speed and a tray.";
        }
        else if (speedSelected == 0)
        {
            warningMessage = "Please select a speed.";
        }
        else if (currentTray == nullptr)
        {
            warningMessage = "Please select a tray.";
        }
        if (!warningMessage.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(warningMessage);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
        else
        {
            //stateRun1();
            stateBuzzerDelay();
        }
    }
}

void MainWindow::on_pushButtonStartTimer_clicked()
{
    //Recognize start pressed in Run1State and Run2State only
    //RunState 2 requires the timer to be stopped and reset the delay timer
    if (currentState == states::Run1State)
    {
        stateTimeDelay();
    }
    else if (currentState == states::Run2State)
    {
        // //Make sure counterTimer is not running - pretty sure this is wrong
        //        // Timer should be running if belt 3 is running
        // if(counterTimer.isActive())
        //     counterTimer.stop();

        remainingTime = waitTime;
        ui->labelTimer->setText(QString::number(remainingTime));
        stateTimeDelay();
    }
}

void MainWindow::on_actionUpdate_Motor_Factors_triggered()
{
    MotorFactors *motorFactors = new MotorFactors(motors);
    motorFactors->setModal(true);
    connect(motorFactors, &QDialog::accepted, this, &MainWindow::writeMotorJson);
    motorFactors->getMotorFactors();
    motorFactors->exec();
    //motorFactors->show();
    writeMotorJson();
    sendMotorSpeedsToModbus();
}

void MainWindow::on_actionUpdate_Upper_Soil_Belt_Factors_triggered()
{
    UpperSoilBeltFact *upperSoilFactors = new UpperSoilBeltFact(trays);
    upperSoilFactors->setModal(true);
    connect(upperSoilFactors, &QDialog::accepted, this, &MainWindow::writeMotorJson);
    upperSoilFactors->getTrayMotor8Factors();
    upperSoilFactors->exec();
    writeUpperSoilBeltJson();
    sendMotorSpeedsToModbus();
}

void MainWindow::on_actionUpdate_COM_Ports_triggered()
{
    UpdateCOMPorts *comPorts = new UpdateCOMPorts();
    comPorts->setModal(true);
    connect(comPorts, &UpdateCOMPorts::sendCOMPorts, this, &MainWindow::updateCOMPorts);
    comPorts->fillCOMPorts(m_inputPortName, m_outputPortName);
    comPorts->exec();
    writeCOMPorts();
}

void MainWindow::on_actionUpdate_Tray_Timing_triggered()
{
    TrayCalibFactors *trayTiming = new TrayCalibFactors(trays);
    trayTiming->setModal(true);
    connect(trayTiming, &QDialog::accepted, this, &MainWindow::writeTrayJson);
    trayTiming->getTrayFactors();
    trayTiming->exec();
    writeTrayJson();
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::on_actionView_Production_Log_triggered()
{
    LogViewer *logViewer = new LogViewer(&productionLog, this);
    logViewer->setModal(true);
    logViewer->exec();
    delete logViewer;
}

void MainWindow::on_actionReset_Total_Counter_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to reset the total counter?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);

    switch (int ret = msgBox.exec())
    {
    case QMessageBox::Yes:m_totalCounter = 0;
        ui->lcdTotalCounter->display(m_totalCounter);
        totalCounter.writeTotalCounter(m_totalCounter);
        break;
    case QMessageBox::No:break;
    default:break;
    }
}

void MainWindow::inputChanged(int address, bool value)
{
    qDebug() << "MainWindow Input: " << address << "status: " << value;

    if (address == startButtonAddress && value)
        on_pushButtonStart_clicked();
    else if (address == stopButtonAddress && !value)  //Stop is NC
        on_pushButtonStop_clicked();
    else if (address == startDelayButtonAddress && value)
        on_pushButtonStartTimer_clicked();
    else if (address == eStopButtonAddress && !value)  //EStop is NC
        stateEstop();
    else if (currentState == states::EstopState && value)
        StateEstopCleared();
}

void MainWindow::sendMotorSpeedsToModbus()
{
    //send motor speeds to modbus
    writeAnalogOutput(motor1->analogAddress(), motor1->speed());
    writeAnalogOutput(motor2->analogAddress(), motor2->speed());
    writeAnalogOutput(motor3->analogAddress(), motor3->speed());
    writeAnalogOutput(motor4->analogAddress(), motor4->speed());
    writeAnalogOutput(motor5->analogAddress(), motor5->speed());
    writeAnalogOutput(motor6->analogAddress(), motor6->speed());
    writeAnalogOutput(motor8->analogAddress(), motor8->speed());
}

void MainWindow::handleSpeedChange(int speed, const QString& displayColor)
{
    // Common speed change logic for all speed buttons
    ui->frameDisplay->setStyleSheet(QString("background-color: %1;").arg(displayColor));
    speedSelected = speed;
    
    //Update counter interval based on speed selected
    if (currentTray != nullptr)
    {
        counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
        if (currentState == states::Run2State)
            timerCounter.start(static_cast<int>(counterTimerInterval * 1000.0));
        setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    }
    else {
        qWarning() << "Cannot change speed: No tray selected";
    }
}

// TIMER-BASED COUNTING: Increment plant count
// Called by timerCounter timeout - each tick = one plant
// Timer interval pre-calculated based on belt speed, tray type, and speed setting
// TIMER-BASED COUNTING: Increment plant count
// Called by timerCounter timeout - each tick = one plant
// Timer interval pre-calculated based on belt speed, tray type, and speed setting
void MainWindow::incrementCurrentCounter()
{
    QMutexLocker locker(&m_stateMutex);  // Thread-safe counter increment
    
    m_totalCounter++;         // Running total since last reset
    m_currentCounter++;       // Current production run count
    m_countersSinceLastWrite++;  // Batch write optimization
    
    ui->lcdTotalCounter->display(m_totalCounter);
    ui->lcdCurrentCounter->display(m_currentCounter);

    // Batched write: only write to disk every N counts to reduce I/O
    if (m_countersSinceLastWrite >= m_counterWriteBatchSize) {
        saveCounterToDisk();
        m_countersSinceLastWrite = 0;
    }
}

void MainWindow::saveCounterToDisk()
{
    // Save counter to disk - called periodically or on important events
    totalCounter.writeTotalCounter(m_totalCounter);
    qDebug() << "Counter saved to disk:" << m_totalCounter;
}

void MainWindow::logProductionRun()
{
    // Log production run data: count, speed, tray, date/time
    if (m_currentCounter > 0 && currentTray != nullptr) {
        productionLog.addEntry(
            m_currentCounter,
            speedSelected,
            currentTray->getName(),
            m_totalCounter
        );
        qInfo() << "Production run logged - Count:" << m_currentCounter
                << "Speed:" << speedSelected
                << "Tray:" << currentTray->getName()
                << "Total:" << m_totalCounter;
    } else if (m_currentCounter > 0) {
        // Log even if no tray selected
        productionLog.addEntry(
            m_currentCounter,
            speedSelected,
            "No Tray Selected",
            m_totalCounter
        );
        qInfo() << "Production run logged (no tray) - Count:" << m_currentCounter;
    }
}

// Calculate timer interval for plant counting
// TIMER-BASED COUNTING: Each tray type has calibrated time factors for each speed
// Time factor = seconds between plants at that speed setting
// Example: Tray 1 at Speed 3 might be 0.5 seconds (2 plants/second)
//          Tray 2 at Speed 6 might be 0.2 seconds (5 plants/second)
// Calibration done via Tray Calibration Factors dialog
// Calculate timer interval for plant counting
// TIMER-BASED COUNTING: Each tray type has calibrated time factors for each speed
// Time factor = seconds between plants at that speed setting
// Example: Tray 1 at Speed 3 might be 0.5 seconds (2 plants/second)
//          Tray 2 at Speed 6 might be 0.2 seconds (5 plants/second)
// Calibration done via Tray Calibration Factors dialog
double MainWindow::getCounterTimerInterval(int speedSelected, const QSharedPointer<Tray> &currentTray)
{
    switch (speedSelected)
    {
    case 1:return currentTray->getSpeed1TimeFactor();
    case 2:return currentTray->getSpeed2TimeFactor();
    case 3:return currentTray->getSpeed3TimeFactor();
    case 4:return currentTray->getSpeed4TimeFactor();
    case 5:return currentTray->getSpeed5TimeFactor();
    case 6:return currentTray->getSpeed6TimeFactor();
    default:return 0.0;
    }
}

void MainWindow::on_pushButtonTray1_clicked()
{
    //6-06 Tray
    currentTray = tray1;
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    ui->pushButtonTray1->setStyleSheet("background-color: lightgrey; border: 8px solid white;");
    ui->pushButtonTray2->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray3->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray4->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray5->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray6->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    //readUpperSoilBeltJson(currentTray);
    setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::on_pushButtonTray2_clicked()
{
    //3.5" tray
    currentTray = tray2;
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    ui->pushButtonTray1->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray2->setStyleSheet("background-color: lightgrey; border: 8px solid white;");
    ui->pushButtonTray3->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray4->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray5->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray6->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    //readUpperSoilBeltJson(currentTray);
    setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::on_pushButtonTray3_clicked()
{
    //4.5" tray
    currentTray = tray3;
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    ui->pushButtonTray1->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray2->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray3->setStyleSheet("background-color: lightgrey; border: 8px solid white;");
    ui->pushButtonTray4->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray5->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray6->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    //readUpperSoilBeltJson(currentTray);
    setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::on_pushButtonTray4_clicked()
{
    //5" Tray
    currentTray = tray4;
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    ui->pushButtonTray1->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray2->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray3->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray4->setStyleSheet("background-color: lightgrey; border: 8px solid white;");
    ui->pushButtonTray5->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray6->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    //readUpperSoilBeltJson(currentTray);
    setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::on_pushButtonTray5_clicked()
{
    //Gallon Tray
    currentTray = tray5;
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    ui->pushButtonTray1->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray2->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray3->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray4->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray5->setStyleSheet("background-color: lightgrey; border: 8px solid white;");
    ui->pushButtonTray6->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    //readUpperSoilBeltJson(currentTray);
    setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::on_pushButtonTray6_clicked()
{
    //8" Tray
    currentTray = tray6;
    counterTimerInterval = getCounterTimerInterval(speedSelected, currentTray);
    ui->pushButtonTray1->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray2->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray3->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray4->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray5->setStyleSheet("background-color: lightgrey; border: 2px solid grey;");
    ui->pushButtonTray6->setStyleSheet("background-color: lightgrey; border: 8px solid white;");
    //readUpperSoilBeltJson(currentTray);
    setMotorSpeeds(baseSpeed, speedSelected, currentTray);
    stateRun2UpdateTimer();
}

void MainWindow::startButtonChanged(quint16 onOff)
{
    if (onOff)
        on_pushButtonStart_clicked();
}

void MainWindow::stopButtonChanged(quint16 onOff)
{
    if (!onOff)  //Stop is NC
        on_pushButtonStop_clicked();
}

void MainWindow::startDelayButtonChanged(quint16 onOff)
{
    if (onOff)
        on_pushButtonStartTimer_clicked();
}

void MainWindow::eStopButtonChanged(quint16 onOff)
{
    if (!onOff)  //EStop is NC
        stateEstop();
    else if (currentState == states::EstopState && onOff)
        StateEstopCleared();
}

void MainWindow::turn_all_outputs_off()
{
    qInfo() << "Turning all outputs off";
    for (int var = 0; var < 16; ++var) {
        writeDigitalOutput(var, 0);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question(
        this,
        "Bonnie Plants Conveyor Interface",
        tr("Are you sure you want to quit the application?\n"),
        QMessageBox::Cancel | QMessageBox::Yes,
        QMessageBox::Yes
        );

    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        //Enter Stop State before close - need to test
        stateStop();
        
        // Ensure counter is saved before exit
        if (m_countersSinceLastWrite > 0) {
            saveCounterToDisk();
            qInfo() << "Counter saved on application exit";
        }
        
        event->accept();
    }
}

// ========== TEST MODE FUNCTIONS ==========

void MainWindow::setupTestModeUI()
{
    // Create test mode control panel
    QGroupBox* testPanel = new QGroupBox("Test Mode - Input Simulation", this);
    testPanel->setStyleSheet("QGroupBox { font-weight: bold; background-color: #ffffcc; border: 2px solid orange; }");
    
    QVBoxLayout* layout = new QVBoxLayout();
    
    QPushButton* btnSimRun1 = new QPushButton("Simulate RUN1 Button", testPanel);
    btnSimRun1->setStyleSheet("QPushButton { background-color: #90EE90; padding: 10px; }");
    connect(btnSimRun1, &QPushButton::clicked, this, &MainWindow::simulateRun1Button);
    layout->addWidget(btnSimRun1);
    
    QPushButton* btnSimStop = new QPushButton("Simulate STOP Button", testPanel);
    btnSimStop->setStyleSheet("QPushButton { background-color: #FFB6C1; padding: 10px; }");
    connect(btnSimStop, &QPushButton::clicked, this, &MainWindow::simulateStopButton);
    layout->addWidget(btnSimStop);
    
    QPushButton* btnSimStartTimer = new QPushButton("Simulate START DELAY Button", testPanel);
    btnSimStartTimer->setStyleSheet("QPushButton { background-color: #87CEEB; padding: 10px; }");
    connect(btnSimStartTimer, &QPushButton::clicked, this, &MainWindow::simulateStartDelayButton);
    layout->addWidget(btnSimStartTimer);
    
    QPushButton* btnSimEStop = new QPushButton("Simulate E-STOP (Emergency)", testPanel);
    btnSimEStop->setStyleSheet("QPushButton { background-color: #FF6B6B; color: white; padding: 10px; font-weight: bold; }");
    connect(btnSimEStop, &QPushButton::clicked, this, &MainWindow::simulateEStop);
    layout->addWidget(btnSimEStop);
    
    testPanel->setLayout(layout);
    
    // Position the panel in the main window (adjust coordinates based on your UI)
    testPanel->setGeometry(10, 400, 250, 180);
    testPanel->show();
    
    qInfo() << "Test mode UI controls added";
}

void MainWindow::simulateRun1Button()
{
    qInfo() << "[TEST MODE] Simulating RUN1 button press";
    on_pushButtonStart_clicked();
}

void MainWindow::simulateStopButton()
{
    qInfo() << "[TEST MODE] Simulating STOP button press";
    on_pushButtonStop_clicked();
}

void MainWindow::simulateStartDelayButton()
{
    qInfo() << "[TEST MODE] Simulating START DELAY button press";
    on_pushButtonStartTimer_clicked();
}

void MainWindow::simulateEStop()
{
    qInfo() << "[TEST MODE] Simulating E-STOP activation";
    if (currentState != states::EstopState) {
        stateEstop();
    } else {
        qInfo() << "[TEST MODE] E-STOP already active, clearing it";
        StateEstopCleared();
    }
}
