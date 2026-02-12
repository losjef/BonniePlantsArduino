#ifndef TRAY_H
#define TRAY_H

#include <QObject>

class Tray : public QObject
{
    Q_OBJECT
public:
    explicit Tray(QObject *parent = nullptr);

    const QString &getName() const;
    void setName(const QString &name);

    double getSpeed1TimeFactor() const;
    void setSpeed1TimeFactor(double speed1TimeFactor);

    double getSpeed2TimeFactor() const;
    void setSpeed2TimeFactor(double speed2TimeFactor);

    double getSpeed3TimeFactor() const;
    void setSpeed3TimeFactor(double speed3TimeFactor);

    double getSpeed4TimeFactor() const;
    void setSpeed4TimeFactor(double speed4TimeFactor);

    double getSpeed5TimeFactor() const;
    void setSpeed5TimeFactor(double speed5TimeFactor);

    double getSpeed6TimeFactor() const;
    void setSpeed6TimeFactor(double speed6TimeFactor);

    double motor8SpeedCalFactor1() const;
    void setMotor8SpeedCalFactor1(double newMotor8SpeedCalFactor1);

    double motor8SpeedCalFactor2() const;
    void setMotor8SpeedCalFactor2(double newMotor8SpeedCalFactor2);

    double motor8SpeedCalFactor3() const;
    void setMotor8SpeedCalFactor3(double newMotor8SpeedCalFactor3);

    double motor8SpeedCalFactor4() const;
    void setMotor8SpeedCalFactor4(double newMotor8SpeedCalFactor4);

    double motor8SpeedCalFactor5() const;
    void setMotor8SpeedCalFactor5(double newMotor8SpeedCalFactor5);

    double motor8SpeedCalFactor6() const;
    void setMotor8SpeedCalFactor6(double newMotor8SpeedCalFactor6);

private:
    QString name;

    //Tray timing factors
    double speed1TimeFactor {1.0};
    double speed2TimeFactor {1.0};
    double speed3TimeFactor {1.0};
    double speed4TimeFactor {1.0};
    double speed5TimeFactor {1.0};
    double speed6TimeFactor {1.0};

    //Motor 8 - upper soil belt speeds - these depend on which tray is selected
    double m_motor8SpeedCalFactor1 = 1.0;
    double m_motor8SpeedCalFactor2 = 1.0;
    double m_motor8SpeedCalFactor3 = 1.0;
    double m_motor8SpeedCalFactor4 = 1.0;
    double m_motor8SpeedCalFactor5 = 1.0;
    double m_motor8SpeedCalFactor6 = 1.0;

};

#endif // TRAY_H
