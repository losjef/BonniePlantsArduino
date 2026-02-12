#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSerialBus>
#include <QModbusClient>
#include <QModbusRtuSerialClient>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

#include <QTimer>

#include <QThread>
#include <QSharedPointer>
#include <QMutex>

#include "motor.h"
#include "scaninputs.h"
#include "Counter.h"
#include "tray.h"
#include "ProductionLog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	// Test mode support
	bool isTestMode() const { return m_testMode; }
	void setupTestModeUI();

private:
	Ui::MainWindow* ui;

	// State machine states for conveyor control
	enum states
	{
		StopState,        // Motors stopped, system idle
		Run1State,        // Initial run state, waiting for time delay
		TimeDelayState,   // Countdown timer before Run2
		Run2State,        // Main production state with counting
		EstopState,       // Emergency stop activated
		BuzzerDelayState  // Buzzer active after countdown
	};

	// === Core Variables ===
	int speedSelected{ 0 };      // Current speed setting (0-6)
	int remainingTime{ 60 };     // Countdown timer value (seconds)
	int waitTime{ 60 };          // Configured wait time for TimeDelay state
	int m_totalCounter{ 0 };     // Total plants counted since last reset
	int m_currentCounter{ 0 };   // Current production run count
	const int analogTypeCode{ 32 };   // Modbus function code for analog writes
	int currentState{ StopState };    // Active state machine state
	int previousState{ StopState };   // State before E-stop (for recovery)
	int estopReturnToState{ StopState };  // Target state after E-stop clear
	
	// === System Configuration Constants ===
	static constexpr int NUM_SPEEDS = 6;                    // Total speed settings available
	static constexpr int COUNTER_BATCH_SIZE_DEFAULT = 10;   // Disk writes every N counts
	static constexpr int READ_WRITE_DELAY_MS = 100;         // Delay between Modbus operations
	static constexpr int BUZZER_DELAY_SECONDS = 3;          // Buzzer duration
	static constexpr double BASE_SPEED_PERCENT = 100.0;     // Base speed calibration (100% = 10V)
	
	const int readWriteDelay{ READ_WRITE_DELAY_MS };  // Modbus timing delay

	// Base speed for motor calibration (all motors use same base for 0-10V calibration)
    const double baseSpeed{ BASE_SPEED_PERCENT };
    // Legacy commented code - all base speeds unified to single value
    // const double baseSpeed2{ 100.0 };
    // const double baseSpeed3{ 100.0 };
    // const double baseSpeed4{ 100.0 };
    // const double baseSpeed5{ 100.0 };
    // const double baseSpeed6{ 100.0 };

	// === Multi-threading ===
	// Input scanning runs on separate thread to avoid blocking UI
	void CreateInputScanThread();
	ScanInputs myInputScan;     // Worker object for input scanning
	QThread inputScanThread;    // Dedicated thread for continuous input polling

	// === Core Functions ===
	void countDownTimerDecrement();  // TimeDelay state countdown handler
    int writeDigitalOutput(quint16 address, int onOff);  // Modbus digital output (motor on/off)
	void handleDOReplyFinished();  // Async response handler for digital outputs
	int writeAnalogOutput(int motorAddress, int percent);  // Modbus analog output (motor speed 0-100%)
	int calculateAnalogValue(int percent);  // Convert percent to Modbus value (0-32767)
	void adjustWaitTime(int adjustment);  // Increment/decrement wait timer
	void sendMotorSpeedsToModbus();  // Batch send all motor speeds
	void handleSpeedChange(int speed, const QString& displayColor);  // Unified speed button handler
	
	// Timer-based counting calculation
	// Returns interval (seconds) between plant counts based on belt speed and tray type
	static double getCounterTimerInterval(int speedSelected, const QSharedPointer<Tray>& currentTray);
    void setMotorSpeeds(double baseSpeed, int motorCalFactors, const QSharedPointer<Tray>& currentTray);
    void stateRun2UpdateTimer();  // Timer callback for Run2 plant counting

	// === State Machine Functions ===
	void stateStop();          // Handle Stop button press
	void stateRun1();          // Initial run state
	void stateTimeDelay();     // Countdown delay before Run2
	void stateRun2();          // Main production run with counting
	void stateEstop();         // Emergency stop (critical safety state)
	void stateBuzzerDelay();   // Buzzer active after countdown

	// State transition events
	void StateTimeDelayTimerComplete();  // TimeDelay → Run2 transition
	void StateEstopCleared();            // E-stop released, return to previous state
	void buzzerTimerDecrement();         // BuzzerDelay countdown

	// === JSON Configuration Files ===
	const QString fileNameMotor = "MotorCalibFact.json";           // Motor speed calibration factors
	const QString fileNameTimer = "CountDownTimer.json";           // Wait time settings
	const QString fileNameTotalCounter = "TotalCounter.json";      // Persistent counter storage
	const QString fileNameTrayFactors = "TrayFactors.json";        // Tray-specific timing
	const QString fileNameComPorts = "COMPorts.json";              // Serial port configuration
	const QString fileNameUpperSoilBelt = "UpperSoilBeltFactors.json";  // Motor 8 tray factors

	// JSON read/write helpers
	void writeJson(QJsonObject& obj, const QString& fileName);
	void readJson(QJsonObject& obj, const QString& fileName);
	void writeMotorJson();
	void readMotorJson();
	void writeUpperSoilBeltJson();
    void readUpperSoilBeltJson(QSharedPointer<Tray> tray);

	// === Motor Objects (8 conveyor motors) ===
	QSharedPointer<Motor> motor1;   // Infeed Belt
	QSharedPointer<Motor> motor2;   // Lower Soil Belt
	QSharedPointer<Motor> motor3;   // Flat Filler Belt
	QSharedPointer<Motor> motor4;   // Planting Line Belt
	QSharedPointer<Motor> motor5;   // Reserved/Future use
	QSharedPointer<Motor> motor6;   // Reserved/Future use
	QSharedPointer<Motor> motor7;   // Reserved/Future use
	QSharedPointer<Motor> motor8;   // Upper Soil Belt (tray-specific calibration)
	QHash<QString, QSharedPointer<Motor>> motors;  // Motor lookup by name

	//Trays
	void writeTrayJson();
	void readTrayJson();
	QSharedPointer<Tray> currentTray = nullptr;

	QSharedPointer<Tray> tray1; //6-06
	QSharedPointer<Tray> tray2; //3.5
	QSharedPointer<Tray> tray3; //4.5
	QSharedPointer<Tray> tray4; //5
	QSharedPointer<Tray> tray5; //Gallon
	QSharedPointer<Tray> tray6; //8
	QHash<QString, QSharedPointer<Tray>> trays;

	// Total Counter
	Counter totalCounter;
	double totalCounterInterval{ 0.5 };  //Need function to read/write from json file
	void incrementCurrentCounter();
	void saveCounterToDisk();  // Batched save function
	// === Counter Optimization (Reduced Disk I/O) ===
	void logProductionRun();  // Log production data to CSV before counter reset
	int m_counterWriteBatchSize{ COUNTER_BATCH_SIZE_DEFAULT };  // Batch size: write every N counts
	int m_countersSinceLastWrite{ 0 };  // Tracks counts since last disk write

	// Production logging system (CSV-based)
	ProductionLog productionLog;

	// === Thread Safety ===
	QMutex m_stateMutex;  // Mutex to protect state changes between main and input scan threads

	// === Test Mode ===
	bool m_testMode{ false };  // Test mode enabled via CONVEYOR_TEST_MODE=1 environment variable
	void simulateEStop();       // Test mode: simulate E-stop input
	void simulateRun1Button();  // Test mode: simulate Run1 button
	void simulateStopButton();  // Test mode: simulate Stop button
	void simulateStartDelayButton();  // Test mode: simulate Start Delay button

	// === Timer-Based Plant Counting System ===
	// Plant counting is TIMER-BASED, not sensor-based
	// Timer interval calculated from: belt speed + tray type + speed setting
	// Each timer tick = one plant passing through system
	QTimer timerMotors;    // Countdown timer for TimeDelay state (1 second ticks)
	QTimer timerCounter;   // Triggers plant count increments (interval varies by speed/tray)
	QTimer timerBuzzer;    // Buzzer duration timer
	void writeTimerJson(int newDelay);
	void readTimerJson();
	double counterTimerInterval{ 0.1 };  // Calculated interval between plants (seconds)
	                                      // Formula: tray-specific time factor for selected speed

	int buzzerTimerDuration{ BUZZER_DELAY_SECONDS };  // Buzzer duration in seconds

	// === Modbus RTU Communication ===
    QString m_outputPortName{ "COM4" };  // Default output port (digital + analog)
    QString m_inputPortName{ "COM5" };   // Default input port (digital inputs)
	void readCOMPorts();   // Load COM port config from JSON
	void writeCOMPorts();  // Save COM port config to JSON
	void updateCOMPorts(QString NewInputPort, QString NewOutputPort);  // Update and persist ports
	int createQModbusRtuSerialClient();  // Initialize Modbus client (57600 baud, 8N1)
	QSharedPointer<QModbusRtuSerialClient> modbusClient1;  // Shared Modbus client for I/O
	QModbusDataUnit writeAnalogOut;  // Analog output data unit (defined in writeanalogoutput.cpp)

	// Modbus device addresses
	// TODO: Update these with actual Waveshare module addresses when hardware specs available
	const int m_digitalOutAddress{ 1 };
	const int m_digitalInAddress{ 2 };
	const int m_analogOutAddress{ 3 };

	//Set input addresses
    const int startButtonAddress{ 0 };                  //Input Terminal 1
    const int stopButtonAddress{ 1 };                   //Input Terminal 2
    const int startDelayButtonAddress{ 2 };             //Input Terminal 3
    const int eStopButtonAddress{ 3 };                  //INput Terminal 4

	//    //Set output addresses
	//    //TODO: Might want to sequence these better - but will have to rewire prototype unit
	//    //TODO: Waveshare devices will need IO number and device ID
    const quint16 m_motor1DigitalOutAddress{ 0 };       //Infeed Belt - Output Relay
    const quint16 m_motor1AnalogOutAddress{ 0 };        //Infeed Belt 0-10V - Output Terminal 9
	const quint16 m_motor1ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor1ModbusAnalogDeviceID{ 3 };
	QString m_motor1Name = "Infeed Belt";
    const quint16 m_motor2DigitalOutAddress{ 1 };       //Lower Soil Belt - Output Relay
    const quint16 m_motor2AnalogOutAddress{ 1 };        //Lower Soil Belt 0-10V - Output Terminal 10
	const quint16 m_motor2ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor2ModbusAnalogDeviceID{ 3 };
	QString m_motor2Name = "Lower Soil Belt";
    const quint16 m_motor3DigitalOutAddress{ 2 };       //Flat Filler Belt - Output Relay
    const quint16 m_motor3AnalogOutAddress{ 2 };        //Flat Filler Belt 0-10V - Output Terminal 11
	const quint16 m_motor3ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor3ModbusAnalogDeviceID{ 3 };
	QString m_motor3Name = "Flat Filler Belt";
    const quint16 m_motor4DigitalOutAddress{ 3 };       //Planting Line Belt - Output Relay
    const quint16 m_motor4AnalogOutAddress{ 3 };        //Planting Line Belt 0-10V - Output Terminal 12
	const quint16 m_motor4ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor4ModbusAnalogDeviceID{ 3 };
	QString m_motor4Name = "Planting Line Belt";
    const quint16 m_motor5DigitalOutAddress{ 4 };       //Future Motor - Output Relay
    const quint16 m_motor5AnalogOutAddress{ 4 };        //Future Motor 0-10V - Output Terminal 13
	const quint16 m_motor5ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor5ModbusAnalogDeviceID{ 3 };
    QString m_motor5Name = "Motor 5";
    const quint16 m_motor6DigitalOutAddress{ 5 };       //Future Motor - Output Relay
    const quint16 m_motor6AnalogOutAddress{ 5 };        //Future Motor 0-10V - Output Terminal 14
	const quint16 m_motor6ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor6ModbusAnalogDeviceID{ 3 };
	QString m_motor6Name = "Motor 6";
	//  const quint16 m_motor7DigitalOutAddress {6};
    const quint16 m_motor8DigitalOutAddress{ 7 };       //Upper Soil Belt - Output Relay
    const quint16 m_motor8AnalogOutAddress{ 7 };        //Upper Soild Belt 0-10V - Output Terminal 15
	const quint16 m_motor8ModbusDigitalDeviceID{ 1 };
	const quint16 m_motor8ModbusAnalogDeviceID{ 3 };
	QString m_motor8Name = "Upper Soil Belt";

    const quint16 m_redLightDigitalOutAddress{ 8 };             //Output Terminal 5
    const quint16 m_redLightModbusDigitalDeviceID{ 1 };
    const quint16 m_yellowLightDigitalOutAddress{ 9 };          //Ouput Terminal 6
	const quint16 m_yellowLightModbusDigitalDeviceID{ 1 };
    const quint16 m_greenLightDigitalOutAddress{ 10 };          //Output Terminal 7
	const quint16 m_greenLightModbusDigitalDeviceID{ 1 };
    const quint16 m_buzzerDigitalOutAddress{ 11 };              //Output Terminal 8
	const quint16 m_buzzerModbusDigitalDeviceID{ 1 };

	const quint16 m_EStopOutDigitalOutAddress{ 12 };
	const quint16 m_EStopOutModbusDigitalDeviceID{ 1 };

	QModbusReply* replyDigitalOut;
	QModbusReply* replyAnalogOut;
	//QModbusReply *replyAnalogInitial;

signals:
	void totalCounterChanged();

private slots:
	void on_pushButtonSpeed1_clicked();
	void on_pushButtonSpeed2_clicked();
	void on_pushButtonSpeed3_clicked();
	void on_pushButtonSpeed4_clicked();
	void on_pushButtonSpeed5_clicked();
	void on_pushButtonSpeed6_clicked();

	void on_pushButtonStop_clicked();
	void on_pushButtonStart_clicked();
	void on_pushButtonStartTimer_clicked();

	void on_actionUpdate_Motor_Factors_triggered();
	void on_actionReset_Total_Counter_triggered();
	void on_actionUpdate_Tray_Timing_triggered();
	void on_actionUpdate_COM_Ports_triggered();
    void on_actionUpdate_Upper_Soil_Belt_Factors_triggered();

	void on_pushButtonPlus5_clicked();
	void on_pushButtonPlus1_clicked();
	void on_pushButtonSaveTimer_clicked();
	void on_pushButtonMinus1_clicked();
	void on_pushButtonMinus5_clicked();

	void on_pushButtonTray1_clicked();
	void on_pushButtonTray2_clicked();
	void on_pushButtonTray3_clicked();
	void on_pushButtonTray4_clicked();
	void on_pushButtonTray5_clicked();
	void on_pushButtonTray6_clicked();
	void on_actionView_Production_Log_triggered();

    void turn_all_outputs_off();
    void closeEvent(QCloseEvent *event);

public slots:
	void modbusConnected(bool connected);
	void inputChanged(int address, bool value);
	void startButtonChanged(quint16 onOff);
	void stopButtonChanged(quint16 onOff);
	void startDelayButtonChanged(quint16 onOff);
	void eStopButtonChanged(quint16 onOff);

	void createMembers();
};

#endif // MAINWINDOW_H
