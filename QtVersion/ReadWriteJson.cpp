#include "mainwindow.h"

void MainWindow::writeJson(QJsonObject &obj, const QString &fileName)
{
    QJsonDocument jDoc(obj);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Could not write file: " << fileName;
        qCritical() << file.errorString();
        return;
    }
    file.write(jDoc.toJson());
    qInfo() << "FileWritten: " << fileName;
    file.close();
}

void MainWindow::readJson(QJsonObject &obj, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Could not read file: " << fileName;
        qCritical() << file.errorString();
        return;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument jDoc = QJsonDocument::fromJson(data);
    obj = jDoc.object();
    qInfo() << "FileRead: " << fileName;
}

void MainWindow::writeMotorJson()
{
    QJsonObject motorObj;
    QJsonArray motorsArr;
    motorObj["name"] = motor1->name();
    motorObj["SpeedFactor1"] = motor1->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor1->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor1->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor1->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor1->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor1->speedCalFactor6();
    motorsArr.append(motorObj);
    motorObj["name"] = motor2->name();
    motorObj["SpeedFactor1"] = motor2->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor2->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor2->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor2->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor2->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor2->speedCalFactor6();
    motorsArr.append(motorObj);
    motorObj["name"] = motor3->name();
    motorObj["SpeedFactor1"] = motor3->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor3->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor3->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor3->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor3->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor3->speedCalFactor6();
    motorsArr.append(motorObj);
    motorObj["name"] = motor4->name();
    motorObj["SpeedFactor1"] = motor4->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor4->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor4->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor4->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor4->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor4->speedCalFactor6();
    motorsArr.append(motorObj);
    motorObj["name"] = motor5->name();
    motorObj["SpeedFactor1"] = motor5->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor5->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor5->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor5->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor5->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor5->speedCalFactor6();
    motorsArr.append(motorObj);
    motorObj["name"] = motor6->name();
    motorObj["SpeedFactor1"] = motor6->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor6->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor6->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor6->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor6->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor6->speedCalFactor6();
    motorsArr.append(motorObj);
    motorObj["name"] = motor8->name();
    motorObj["SpeedFactor1"] = motor8->speedCalFactor1();
    motorObj["SpeedFactor2"] = motor8->speedCalFactor2();
    motorObj["SpeedFactor3"] = motor8->speedCalFactor3();
    motorObj["SpeedFactor4"] = motor8->speedCalFactor4();
    motorObj["SpeedFactor5"] = motor8->speedCalFactor5();
    motorObj["SpeedFactor6"] = motor8->speedCalFactor6();
    motorsArr.append(motorObj);

    QJsonObject myObj;
    myObj["Motors"] = motorsArr;
    // writeJson(myObj, fileNameMotor);

    QJsonDocument jDoc(myObj);
    QFile file(fileNameMotor);
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Could not write motor file!";
        qCritical() << file.errorString();
    }
    else
        qInfo() << "File Open";

    file.write(jDoc.toJson());
    file.close();
    qInfo() << "Motor File Written";

    readMotorJson();

    //TODO: need to make a routine to pass the correct base speed
    //setMotorSpeeds(baseSpeed1);
    //sendMotorSpeedsToModbus();


}

void MainWindow::readMotorJson()
{
    //  QFile file(fileNameMotor);
    //  if (!file.open(QIODevice::ReadOnly))
    //  {
    //	qCritical() << "Could not read motor file!";
    //	qCritical() << file.errorString();
    //	return;
    //  }
    //  else
    //	qInfo() << "Motor Factors File Open";
    //
    //  QByteArray data = file.readAll();
    //  file.close();
    //  QJsonDocument jDoc = QJsonDocument::fromJson(data);
    //  QJsonObject myMotorObj = jDoc.object();

    QJsonObject myMotorObj;
    readJson(myMotorObj, fileNameMotor);
    if (myMotorObj.isEmpty())
    {
        qInfo() << "No Object";
    }
    else
    {
        if (myMotorObj.contains("Motors") && myMotorObj["Motors"].isArray())
        {
            QJsonArray motorsArr = myMotorObj["Motors"].toArray();
            for (auto &&index : motorsArr)
            {
                QJsonObject motorObj = index.toObject();
                if (motorObj["name"].toString() == "Infeed Belt")
                {
                    motor1->setName("Infeed Belt");
                    motor1->setSpeedCalFactor1(motorObj["SpeedFactor1"].toDouble());
                    motor1->setSpeedCalFactor2(motorObj["SpeedFactor2"].toDouble());
                    motor1->setSpeedCalFactor3(motorObj["SpeedFactor3"].toDouble());
                    motor1->setSpeedCalFactor4(motorObj["SpeedFactor4"].toDouble());
                    motor1->setSpeedCalFactor5(motorObj["SpeedFactor5"].toDouble());
                    motor1->setSpeedCalFactor6(motorObj["SpeedFactor6"].toDouble());
                    motors.insert(motor1->name(), motor1);
                }
                else if (motorObj["name"].toString() == "Lower Soil Belt")
                {
                    motor2->setName("Lower Soil Belt");
                    motor2->setSpeedCalFactor1(motorObj["SpeedFactor1"].toDouble());
                    motor2->setSpeedCalFactor2(motorObj["SpeedFactor2"].toDouble());
                    motor2->setSpeedCalFactor3(motorObj["SpeedFactor3"].toDouble());
                    motor2->setSpeedCalFactor4(motorObj["SpeedFactor4"].toDouble());
                    motor2->setSpeedCalFactor5(motorObj["SpeedFactor5"].toDouble());
                    motor2->setSpeedCalFactor6(motorObj["SpeedFactor6"].toDouble());
                    motors.insert(motor2->name(), motor2);
                }
                else if (motorObj["name"].toString() == "Flat Filler Belt")
                {
                    motor3->setName("Flat Filler Belt");
                    motor3->setSpeedCalFactor1(motorObj["SpeedFactor1"].toDouble());
                    motor3->setSpeedCalFactor2(motorObj["SpeedFactor2"].toDouble());
                    motor3->setSpeedCalFactor3(motorObj["SpeedFactor3"].toDouble());
                    motor3->setSpeedCalFactor4(motorObj["SpeedFactor4"].toDouble());
                    motor3->setSpeedCalFactor5(motorObj["SpeedFactor5"].toDouble());
                    motor3->setSpeedCalFactor6(motorObj["SpeedFactor6"].toDouble());
                    motors.insert(motor3->name(), motor3);
                }
                else if (motorObj["name"].toString() == "Planting Line")
                {
                    motor4->setName("Planting Line");
                    motor4->setSpeedCalFactor1(motorObj["SpeedFactor1"].toDouble());
                    motor4->setSpeedCalFactor2(motorObj["SpeedFactor2"].toDouble());
                    motor4->setSpeedCalFactor3(motorObj["SpeedFactor3"].toDouble());
                    motor4->setSpeedCalFactor4(motorObj["SpeedFactor4"].toDouble());
                    motor4->setSpeedCalFactor5(motorObj["SpeedFactor5"].toDouble());
                    motor4->setSpeedCalFactor6(motorObj["SpeedFactor6"].toDouble());
                    motors.insert(motor4->name(), motor4);
                }
                else if (motorObj["name"].toString() == "Motor 5")
                {
                    motor5->setName("Motor 5");
                    motor5->setSpeedCalFactor1(motorObj["SpeedFactor1"].toDouble());
                    motor5->setSpeedCalFactor2(motorObj["SpeedFactor2"].toDouble());
                    motor5->setSpeedCalFactor3(motorObj["SpeedFactor3"].toDouble());
                    motor5->setSpeedCalFactor4(motorObj["SpeedFactor4"].toDouble());
                    motor5->setSpeedCalFactor5(motorObj["SpeedFactor5"].toDouble());
                    motor5->setSpeedCalFactor6(motorObj["SpeedFactor6"].toDouble());
                    motors.insert(motor5->name(), motor5);
                }
                else if (motorObj["name"].toString() == "Motor 6")
                {
                    motor6->setName("Motor 6");
                    motor6->setSpeedCalFactor1(motorObj["SpeedFactor1"].toDouble());
                    motor6->setSpeedCalFactor2(motorObj["SpeedFactor2"].toDouble());
                    motor6->setSpeedCalFactor3(motorObj["SpeedFactor3"].toDouble());
                    motor6->setSpeedCalFactor4(motorObj["SpeedFactor4"].toDouble());
                    motor6->setSpeedCalFactor5(motorObj["SpeedFactor5"].toDouble());
                    motor6->setSpeedCalFactor6(motorObj["SpeedFactor6"].toDouble());
                    motors.insert(motor6->name(), motor6);
                }
                else
                    qInfo() << "No Motor Name Matched";
            }
        }
    }

    return;
}

void MainWindow::writeUpperSoilBeltJson()
{
    QJsonObject trayObj;
    QJsonArray traysArr;
    trayObj["name"] = tray1->getName();
    trayObj["SpeedFactor1"] = tray1->motor8SpeedCalFactor1();
    trayObj["SpeedFactor2"] = tray1->motor8SpeedCalFactor2();
    trayObj["SpeedFactor3"] = tray1->motor8SpeedCalFactor3();
    trayObj["SpeedFactor4"] = tray1->motor8SpeedCalFactor4();
    trayObj["SpeedFactor5"] = tray1->motor8SpeedCalFactor5();
    trayObj["SpeedFactor6"] = tray1->motor8SpeedCalFactor6();
    traysArr.append(trayObj);
    trayObj["name"] = tray2->getName();
    trayObj["SpeedFactor1"] = tray2->motor8SpeedCalFactor1();
    trayObj["SpeedFactor2"] = tray2->motor8SpeedCalFactor2();
    trayObj["SpeedFactor3"] = tray2->motor8SpeedCalFactor3();
    trayObj["SpeedFactor4"] = tray2->motor8SpeedCalFactor4();
    trayObj["SpeedFactor5"] = tray2->motor8SpeedCalFactor5();
    trayObj["SpeedFactor6"] = tray2->motor8SpeedCalFactor6();
    traysArr.append(trayObj);
    trayObj["name"] = tray3->getName();
    trayObj["SpeedFactor1"] = tray3->motor8SpeedCalFactor1();
    trayObj["SpeedFactor2"] = tray3->motor8SpeedCalFactor2();
    trayObj["SpeedFactor3"] = tray3->motor8SpeedCalFactor3();
    trayObj["SpeedFactor4"] = tray3->motor8SpeedCalFactor4();
    trayObj["SpeedFactor5"] = tray3->motor8SpeedCalFactor5();
    trayObj["SpeedFactor6"] = tray3->motor8SpeedCalFactor6();
    traysArr.append(trayObj);
    trayObj["name"] = tray4->getName();
    trayObj["SpeedFactor1"] = tray4->motor8SpeedCalFactor1();
    trayObj["SpeedFactor2"] = tray4->motor8SpeedCalFactor2();
    trayObj["SpeedFactor3"] = tray4->motor8SpeedCalFactor3();
    trayObj["SpeedFactor4"] = tray4->motor8SpeedCalFactor4();
    trayObj["SpeedFactor5"] = tray4->motor8SpeedCalFactor5();
    trayObj["SpeedFactor6"] = tray4->motor8SpeedCalFactor6();
    traysArr.append(trayObj);
    trayObj["name"] = tray5->getName();
    trayObj["SpeedFactor1"] = tray5->motor8SpeedCalFactor1();
    trayObj["SpeedFactor2"] = tray5->motor8SpeedCalFactor2();
    trayObj["SpeedFactor3"] = tray5->motor8SpeedCalFactor3();
    trayObj["SpeedFactor4"] = tray5->motor8SpeedCalFactor4();
    trayObj["SpeedFactor5"] = tray5->motor8SpeedCalFactor5();
    trayObj["SpeedFactor6"] = tray5->motor8SpeedCalFactor6();
    traysArr.append(trayObj);
    trayObj["name"] = tray6->getName();
    trayObj["SpeedFactor1"] = tray6->motor8SpeedCalFactor1();
    trayObj["SpeedFactor2"] = tray6->motor8SpeedCalFactor2();
    trayObj["SpeedFactor3"] = tray6->motor8SpeedCalFactor3();
    trayObj["SpeedFactor4"] = tray6->motor8SpeedCalFactor4();
    trayObj["SpeedFactor5"] = tray6->motor8SpeedCalFactor5();
    trayObj["SpeedFactor6"] = tray6->motor8SpeedCalFactor6();
    traysArr.append(trayObj);


    QJsonObject myObj;
    myObj["Factors"] = traysArr;

    QJsonDocument jDoc(myObj);
    QFile file(fileNameUpperSoilBelt);
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Could not write Upper Soil Belt file!";
        qCritical() << file.errorString();
    }
    else
        qInfo() << "File Open";

    file.write(jDoc.toJson());
    file.close();
    qInfo() << "Upper Soil Belt File Written";

    readUpperSoilBeltJson(currentTray);
}

void MainWindow::readUpperSoilBeltJson(QSharedPointer<Tray> tray)
    // TODO: Needs a parameter (which is the selected tray) to determine which tray to read -
    // only 1 tray should be written to motor 8 - not all trays since they are overwriting each other
    // I think this method will only be called when the pot size is selected - or if the speed is changed - have to be careful that pot size exists when speed is selected
    // might have to make one of the pot sizes a default when program starts
{
    QJsonObject myTrayObj;
    readJson(myTrayObj, fileNameUpperSoilBelt);
    if (myTrayObj.isEmpty())
    {
        qInfo() << "No Object";
    }
    else
    {
        if (myTrayObj.contains("Factors") && myTrayObj["Factors"].isArray())
        {
            QJsonArray traysArr = myTrayObj["Factors"].toArray();
            for (auto &&index : traysArr)
            {
                QJsonObject trayObj = index.toObject();
                if (trayObj["name"].toString() == "6-06")
                {
                    tray1->setName(trayObj["name"].toString());
                    tray1->setMotor8SpeedCalFactor1(trayObj["SpeedFactor1"].toDouble());
                    tray1->setMotor8SpeedCalFactor2(trayObj["SpeedFactor2"].toDouble());
                    tray1->setMotor8SpeedCalFactor3(trayObj["SpeedFactor3"].toDouble());
                    tray1->setMotor8SpeedCalFactor4(trayObj["SpeedFactor4"].toDouble());
                    tray1->setMotor8SpeedCalFactor5(trayObj["SpeedFactor5"].toDouble());
                    tray1->setMotor8SpeedCalFactor6(trayObj["SpeedFactor6"].toDouble());
                    trays.insert(tray1->getName(), tray1);
                }
                else if (trayObj["name"].toString() == "3.5")
                {
                    tray2->setName(trayObj["name"].toString());
                    tray2->setMotor8SpeedCalFactor1(trayObj["SpeedFactor1"].toDouble());
                    tray2->setMotor8SpeedCalFactor2(trayObj["SpeedFactor2"].toDouble());
                    tray2->setMotor8SpeedCalFactor3(trayObj["SpeedFactor3"].toDouble());
                    tray2->setMotor8SpeedCalFactor4(trayObj["SpeedFactor4"].toDouble());
                    tray2->setMotor8SpeedCalFactor5(trayObj["SpeedFactor5"].toDouble());
                    tray2->setMotor8SpeedCalFactor6(trayObj["SpeedFactor6"].toDouble());
                    trays.insert(tray2->getName(), tray2);
                }
                else if (trayObj["name"].toString() == "4.5")
                {
                    tray3->setName(trayObj["name"].toString());
                    tray3->setMotor8SpeedCalFactor1(trayObj["SpeedFactor1"].toDouble());
                    tray3->setMotor8SpeedCalFactor2(trayObj["SpeedFactor2"].toDouble());
                    tray3->setMotor8SpeedCalFactor3(trayObj["SpeedFactor3"].toDouble());
                    tray3->setMotor8SpeedCalFactor4(trayObj["SpeedFactor4"].toDouble());
                    tray3->setMotor8SpeedCalFactor5(trayObj["SpeedFactor5"].toDouble());
                    tray3->setMotor8SpeedCalFactor6(trayObj["SpeedFactor6"].toDouble());
                    trays.insert(tray3->getName(), tray3);
                }
                else if (trayObj["name"].toString() == "5")
                {
                    tray4->setName(trayObj["name"].toString());
                    tray4->setMotor8SpeedCalFactor1(trayObj["SpeedFactor1"].toDouble());
                    tray4->setMotor8SpeedCalFactor2(trayObj["SpeedFactor2"].toDouble());
                    tray4->setMotor8SpeedCalFactor3(trayObj["SpeedFactor3"].toDouble());
                    tray4->setMotor8SpeedCalFactor4(trayObj["SpeedFactor4"].toDouble());
                    tray4->setMotor8SpeedCalFactor5(trayObj["SpeedFactor5"].toDouble());
                    tray4->setMotor8SpeedCalFactor6(trayObj["SpeedFactor6"].toDouble());
                    trays.insert(tray4->getName(), tray4);
                }
                else if (trayObj["name"].toString() == "Gallon")
                {
                    tray5->setName(trayObj["name"].toString());
                    tray5->setMotor8SpeedCalFactor1(trayObj["SpeedFactor1"].toDouble());
                    tray5->setMotor8SpeedCalFactor2(trayObj["SpeedFactor2"].toDouble());
                    tray5->setMotor8SpeedCalFactor3(trayObj["SpeedFactor3"].toDouble());
                    tray5->setMotor8SpeedCalFactor4(trayObj["SpeedFactor4"].toDouble());
                    tray5->setMotor8SpeedCalFactor5(trayObj["SpeedFactor5"].toDouble());
                    tray5->setMotor8SpeedCalFactor6(trayObj["SpeedFactor6"].toDouble());
                    trays.insert(tray5->getName(), tray5);
                }
                else if (trayObj["name"].toString() == "8")
                {
                    tray6->setName(trayObj["name"].toString());
                    tray6->setMotor8SpeedCalFactor1(trayObj["SpeedFactor1"].toDouble());
                    tray6->setMotor8SpeedCalFactor2(trayObj["SpeedFactor2"].toDouble());
                    tray6->setMotor8SpeedCalFactor3(trayObj["SpeedFactor3"].toDouble());
                    tray6->setMotor8SpeedCalFactor4(trayObj["SpeedFactor4"].toDouble());
                    tray6->setMotor8SpeedCalFactor5(trayObj["SpeedFactor5"].toDouble());
                    tray6->setMotor8SpeedCalFactor6(trayObj["SpeedFactor6"].toDouble());
                    trays.insert(tray6->getName(), tray6);
                }
                else
                    qInfo() << "No Tray Name Matched for Motor 8";
            }
        }

    }

    //Do we want to set speeds here?
    motor8->setSpeedCalFactor1(tray->motor8SpeedCalFactor1());
    motor8->setSpeedCalFactor1(tray->motor8SpeedCalFactor2());
    motor8->setSpeedCalFactor1(tray->motor8SpeedCalFactor3());
    motor8->setSpeedCalFactor1(tray->motor8SpeedCalFactor4());
    motor8->setSpeedCalFactor1(tray->motor8SpeedCalFactor5());
    motor8->setSpeedCalFactor1(tray->motor8SpeedCalFactor6());

    return;
}

void MainWindow::writeTimerJson(int newDelay)
{
    QJsonObject timerObj;
    timerObj["WaitTime"] = newDelay;

    QJsonObject myObj;
    myObj["TimerValues"] = timerObj;
    writeJson(myObj, fileNameTimer);

    //  QJsonDocument jDoc(myObj);

    //  QFile file(fileNameTimer);
    //  if (!file.open(QIODevice::WriteOnly))
    //  {
    // qCritical() << "Could not write timer file!";
    // qCritical() << file.errorString();
    //  }
    //  else
    // qInfo() << " Timer File Open";

    //  file.write(jDoc.toJson());
    //  file.close();
    //  qInfo() << "Timer File Written";

}

void MainWindow::readTimerJson()
{
    //  QFile file(fileNameTimer);
    //  if (!file.open(QIODevice::ReadOnly))
    //  {
    // qCritical() << "Could not read file!";
    // qCritical() << file.errorString();
    // return;
    //  }
    //  else
    // qInfo() << "Timer File Open";

    //  QByteArray data = file.readAll();
    //  file.close();

    //  QJsonDocument jDoc = QJsonDocument::fromJson(data);
    //  QJsonObject myTimerObj = jDoc.object();
    QJsonObject myTimerObj;
    readJson(myTimerObj, fileNameTimer);

    if (myTimerObj.isEmpty())
    {
        qInfo() << "No Object";
    }
    else
    {
        if (myTimerObj.contains("TimerValues") && myTimerObj["TimerValues"].isObject())
        {
            myTimerObj = myTimerObj["TimerValues"].toObject();
            remainingTime = myTimerObj["WaitTime"].toInt();
            waitTime = myTimerObj["WaitTime"].toInt();
        }
    }
}

void MainWindow::writeTrayJson()
{
    QJsonObject trayObj;
    QJsonArray traysArr;
    trayObj["name"] = tray1->getName();
    trayObj["TimeFactor1"] = tray1->getSpeed1TimeFactor();
    trayObj["TimeFactor2"] = tray1->getSpeed2TimeFactor();
    trayObj["TimeFactor3"] = tray1->getSpeed3TimeFactor();
    trayObj["TimeFactor4"] = tray1->getSpeed4TimeFactor();
    trayObj["TimeFactor5"] = tray1->getSpeed5TimeFactor();
    trayObj["TimeFactor6"] = tray1->getSpeed6TimeFactor();
    traysArr.append(trayObj);
    trayObj["name"] = tray2->getName();
    trayObj["TimeFactor1"] = tray2->getSpeed1TimeFactor();
    trayObj["TimeFactor2"] = tray2->getSpeed2TimeFactor();
    trayObj["TimeFactor3"] = tray2->getSpeed3TimeFactor();
    trayObj["TimeFactor4"] = tray2->getSpeed4TimeFactor();
    trayObj["TimeFactor5"] = tray2->getSpeed5TimeFactor();
    trayObj["TimeFactor6"] = tray2->getSpeed6TimeFactor();
    traysArr.append(trayObj);
    trayObj["name"] = tray3->getName();
    trayObj["TimeFactor1"] = tray3->getSpeed1TimeFactor();
    trayObj["TimeFactor2"] = tray3->getSpeed2TimeFactor();
    trayObj["TimeFactor3"] = tray3->getSpeed3TimeFactor();
    trayObj["TimeFactor4"] = tray3->getSpeed4TimeFactor();
    trayObj["TimeFactor5"] = tray3->getSpeed5TimeFactor();
    trayObj["TimeFactor6"] = tray3->getSpeed6TimeFactor();
    traysArr.append(trayObj);
    trayObj["name"] = tray4->getName();
    trayObj["TimeFactor1"] = tray4->getSpeed1TimeFactor();
    trayObj["TimeFactor2"] = tray4->getSpeed2TimeFactor();
    trayObj["TimeFactor3"] = tray4->getSpeed3TimeFactor();
    trayObj["TimeFactor4"] = tray4->getSpeed4TimeFactor();
    trayObj["TimeFactor5"] = tray4->getSpeed5TimeFactor();
    trayObj["TimeFactor6"] = tray4->getSpeed6TimeFactor();
    traysArr.append(trayObj);
    trayObj["name"] = tray5->getName();
    trayObj["TimeFactor1"] = tray5->getSpeed1TimeFactor();
    trayObj["TimeFactor2"] = tray5->getSpeed2TimeFactor();
    trayObj["TimeFactor3"] = tray5->getSpeed3TimeFactor();
    trayObj["TimeFactor4"] = tray5->getSpeed4TimeFactor();
    trayObj["TimeFactor5"] = tray5->getSpeed5TimeFactor();
    trayObj["TimeFactor6"] = tray5->getSpeed6TimeFactor();
    traysArr.append(trayObj);
    trayObj["name"] = tray6->getName();
    trayObj["TimeFactor1"] = tray6->getSpeed1TimeFactor();
    trayObj["TimeFactor2"] = tray6->getSpeed2TimeFactor();
    trayObj["TimeFactor3"] = tray6->getSpeed3TimeFactor();
    trayObj["TimeFactor4"] = tray6->getSpeed4TimeFactor();
    trayObj["TimeFactor5"] = tray6->getSpeed5TimeFactor();
    trayObj["TimeFactor6"] = tray6->getSpeed6TimeFactor();
    traysArr.append(trayObj);

    QJsonObject myObj;
    myObj["Trays"] = traysArr;
    writeJson(myObj, fileNameTrayFactors);

    //  QJsonDocument jDoc(myObj);
    //
    //  QFile file(fileNameTrayFactors);
    //  if (!file.open(QIODevice::WriteOnly))
    //  {
    //	qCritical() << "Could not write tray file!";
    //	qCritical() << file.errorString();
    //  }
    //  else
    //	qInfo() << "Tray File Open";
    //
    //  file.write(jDoc.toJson());
    //  file.close();
    //  qInfo() << "Tray File Written";

    readTrayJson();
}

void MainWindow::readTrayJson()
{
    //  QFile file(fileNameTrayFactors);
    //  if (!file.open(QIODevice::ReadOnly))
    //  {
    //	qCritical() << "Could not read tray file!";
    //	qCritical() << file.errorString();
    //	return;
    //  }
    //  else
    //	qInfo() << "Tray File Open";
    //
    //  QByteArray data = file.readAll();
    //  file.close();
    //
    //  QJsonDocument jDoc = QJsonDocument::fromJson(data);
    //  QJsonObject myTrayObj = jDoc.object();
    QJsonObject myTrayObj;
    readJson(myTrayObj, fileNameTrayFactors);

    if (myTrayObj.isEmpty())
    {
        qInfo() << "No Object";
    }
    else
    {
        if (myTrayObj.contains("Trays") && myTrayObj["Trays"].isArray())
        {
            QJsonArray traysArr = myTrayObj["Trays"].toArray();
            for (auto &&index : traysArr)
            {
                QJsonObject trayObj = index.toObject();
                if (trayObj["name"].toString() == "6-06")
                {
                    tray1->setName(trayObj["name"].toString());
                    tray1->setSpeed1TimeFactor(trayObj["TimeFactor1"].toDouble());
                    tray1->setSpeed2TimeFactor(trayObj["TimeFactor2"].toDouble());
                    tray1->setSpeed3TimeFactor(trayObj["TimeFactor3"].toDouble());
                    tray1->setSpeed4TimeFactor(trayObj["TimeFactor4"].toDouble());
                    tray1->setSpeed5TimeFactor(trayObj["TimeFactor5"].toDouble());
                    tray1->setSpeed6TimeFactor(trayObj["TimeFactor6"].toDouble());
                    trays.insert("6-06", tray1);
                }
                else if (trayObj["name"].toString() == "3.5")
                {
                    tray2->setName(trayObj["name"].toString());
                    tray2->setSpeed1TimeFactor(trayObj["TimeFactor1"].toDouble());
                    tray2->setSpeed2TimeFactor(trayObj["TimeFactor2"].toDouble());
                    tray2->setSpeed3TimeFactor(trayObj["TimeFactor3"].toDouble());
                    tray2->setSpeed4TimeFactor(trayObj["TimeFactor4"].toDouble());
                    tray2->setSpeed5TimeFactor(trayObj["TimeFactor5"].toDouble());
                    tray2->setSpeed6TimeFactor(trayObj["TimeFactor6"].toDouble());
                    trays.insert("3.5", tray2);

                }
                else if (trayObj["name"].toString() == "4.5")
                {
                    tray3->setName(trayObj["name"].toString());
                    tray3->setSpeed1TimeFactor(trayObj["TimeFactor1"].toDouble());
                    tray3->setSpeed2TimeFactor(trayObj["TimeFactor2"].toDouble());
                    tray3->setSpeed3TimeFactor(trayObj["TimeFactor3"].toDouble());
                    tray3->setSpeed4TimeFactor(trayObj["TimeFactor4"].toDouble());
                    tray3->setSpeed5TimeFactor(trayObj["TimeFactor5"].toDouble());
                    tray3->setSpeed6TimeFactor(trayObj["TimeFactor6"].toDouble());
                    trays.insert("4.5", tray3);
                }
                else if (trayObj["name"].toString() == "5")
                {
                    tray4->setName(trayObj["name"].toString());
                    tray4->setSpeed1TimeFactor(trayObj["TimeFactor1"].toDouble());
                    tray4->setSpeed2TimeFactor(trayObj["TimeFactor2"].toDouble());
                    tray4->setSpeed3TimeFactor(trayObj["TimeFactor3"].toDouble());
                    tray4->setSpeed4TimeFactor(trayObj["TimeFactor4"].toDouble());
                    tray4->setSpeed5TimeFactor(trayObj["TimeFactor5"].toDouble());
                    tray4->setSpeed6TimeFactor(trayObj["TimeFactor6"].toDouble());
                    trays.insert("5", tray4);
                }
                else if (trayObj["name"].toString() == "Gallon")
                {
                    tray5->setName(trayObj["name"].toString());
                    tray5->setSpeed1TimeFactor(trayObj["TimeFactor1"].toDouble());
                    tray5->setSpeed2TimeFactor(trayObj["TimeFactor2"].toDouble());
                    tray5->setSpeed3TimeFactor(trayObj["TimeFactor3"].toDouble());
                    tray5->setSpeed4TimeFactor(trayObj["TimeFactor4"].toDouble());
                    tray5->setSpeed5TimeFactor(trayObj["TimeFactor5"].toDouble());
                    tray5->setSpeed6TimeFactor(trayObj["TimeFactor6"].toDouble());
                    trays.insert("Gallon", tray5);
                }
                else if (trayObj["name"].toString() == "8")
                {
                    tray6->setName(trayObj["name"].toString());
                    tray6->setSpeed1TimeFactor(trayObj["TimeFactor1"].toDouble());
                    tray6->setSpeed2TimeFactor(trayObj["TimeFactor2"].toDouble());
                    tray6->setSpeed3TimeFactor(trayObj["TimeFactor3"].toDouble());
                    tray6->setSpeed4TimeFactor(trayObj["TimeFactor4"].toDouble());
                    tray6->setSpeed5TimeFactor(trayObj["TimeFactor5"].toDouble());
                    tray6->setSpeed6TimeFactor(trayObj["TimeFactor6"].toDouble());
                    trays.insert("8", tray6);
                }
                else
                    qInfo() << "No Tray Name Matched";
            }
        }
    }
}

void MainWindow::readCOMPorts()
{
    //  QFile file(fileNameComPorts);
    //  if (!file.open(QIODevice::ReadOnly))
    //  {
    //	qCritical() << "Could not read COM file!";
    //	qCritical() << file.errorString();
    //	return;
    //  }
    //  else
    //	qInfo() << "COM File Open";
    //
    //  QByteArray data = file.readAll();
    //  file.close();
    //
    //  QJsonDocument jDoc = QJsonDocument::fromJson(data);
    //  QJsonObject myCOMObj = jDoc.object();
    QJsonObject myCOMObj;
    readJson(myCOMObj, fileNameComPorts);
    if (myCOMObj.isEmpty())
    {
        qInfo() << "No Object";
    }
    else
    {
        if (myCOMObj.contains("COMPorts") && myCOMObj["COMPorts"].isArray())
        {
            QJsonArray COMMsArr = myCOMObj["COMPorts"].toArray();
            for (auto &&index : COMMsArr)
            {
                QJsonObject COMObj = index.toObject();
                if (COMObj["name"].toString() == "OutputCom")
                {
                    m_outputPortName = COMObj["Port"].toString();
                }
                else if (COMObj["name"].toString() == "InputCom")
                {
                    m_inputPortName = COMObj["Port"].toString();
                }
                else
                    qInfo() << "No COM Name Matched";
            }
        }
    }
}

void MainWindow::writeCOMPorts()
{
    QJsonObject COMObj;
    QJsonArray COMArr;
    COMObj["name"] = "OutputCom";
    COMObj["Port"] = m_outputPortName;
    COMArr.append(COMObj);
    COMObj["name"] = "InputCom";
    COMObj["Port"] = m_inputPortName;
    COMArr.append(COMObj);

    QJsonObject myObj;
    myObj["COMPorts"] = COMArr;
    writeJson(myObj, fileNameComPorts);

    //  QJsonDocument jDoc(myObj);
    //
    //  QFile file(fileNameComPorts);
    //  if (!file.open(QIODevice::WriteOnly))
    //  {
    //	qCritical() << "Could not write COM file!";
    //	qCritical() << file.errorString();
    //  }
    //  else
    //	qInfo() << " COM File Open";
    //
    //  file.write(jDoc.toJson());
    //  file.close();
    //  qInfo() << "COM File Written";

}

void MainWindow::updateCOMPorts(QString NewInputPort, QString NewOutputPort)
{
    m_inputPortName = NewInputPort;
    m_outputPortName = NewOutputPort;
    writeCOMPorts();
}



