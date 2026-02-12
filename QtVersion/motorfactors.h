#ifndef MOTORFACTORS_H
#define MOTORFACTORS_H

#include <QDialog>
#include "motor.h"
#include "qabstractbutton.h"

namespace Ui {
class MotorFactors;
}

class MotorFactors : public QDialog
{
    Q_OBJECT

public:
    //explicit MotorFactors(QHash<QString, Motor*> motors, QWidget *parent = nullptr);
    explicit MotorFactors(QHash<QString, QSharedPointer<Motor>> motors, QWidget *parent = nullptr);
    ~MotorFactors();

    void getMotorFactors();

private slots:
    void on_buttonBox_accepted();

    //void on_tableWidget_cellChanged(int row, int column);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::MotorFactors *ui;
    QHash<QString, QSharedPointer<Motor>> motors;
};

#endif // MOTORFACTORS_H
