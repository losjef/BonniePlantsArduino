#include "motorfactors.h"
#include "ui_motorfactors.h"

MotorFactors::MotorFactors(QHash<QString, QSharedPointer<Motor> > newMotors, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorFactors)
{
    ui->setupUi(this);
    this->motors = newMotors;
    ui->tableWidget->setColumnCount(6);
}

MotorFactors::~MotorFactors()
{
    delete ui;
}

void MotorFactors::getMotorFactors()
{
    //QHashIterator<QString, QSharedPointer<Motor>> i(motors);
    QHashIterator i(motors);  //Sonar suggested to use QHashIterator without template
    while(i.hasNext())
    {
        i.next();
        for(int j = 0; j < ui->tableWidget->rowCount(); j++)
        {
            if(ui->tableWidget->verticalHeaderItem(j)->text() == i.key())
            {
                QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i.value()->speedCalFactor1()));
                ui->tableWidget->setItem(j,0, item1);
                QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(i.value()->speedCalFactor2()));
                ui->tableWidget->setItem(j,1, item2);
                QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(i.value()->speedCalFactor3()));
                ui->tableWidget->setItem(j,2, item3);
                QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(i.value()->speedCalFactor4()));
                ui->tableWidget->setItem(j,3, item4);
                QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(i.value()->speedCalFactor5()));
                ui->tableWidget->setItem(j,4, item5);
                QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(i.value()->speedCalFactor6()));
                ui->tableWidget->setItem(j,5, item6);
            }
        }
    }
}

void MotorFactors::on_buttonBox_accepted()
{
    // Might be a difference with MinGW and MSVC, but on_button_accepted is running after the writeJson connection in MainWindow - and the wrong values were written

    // for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    // {
    //     motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor1(ui->tableWidget->item(i,0)->text().toDouble());
    //     motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor2(ui->tableWidget->item(i,1)->text().toDouble());
    //     motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor3(ui->tableWidget->item(i,2)->text().toDouble());
    //     motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor4(ui->tableWidget->item(i,3)->text().toDouble());
    //     motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor5(ui->tableWidget->item(i,4)->text().toDouble());
    //     motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor6(ui->tableWidget->item(i,5)->text().toDouble());
    // }
}

// void MotorFactors::on_tableWidget_cellChanged(int row, int column)
// {
//     Set motor values when the numbers are changed in the table
//     switch (column) {
//     case 0:
//         motors.value(ui->tableWidget->verticalHeaderItem(row)->text())->setSpeedCalFactor1(ui->tableWidget->item(row,0)->text().toDouble());
//         break;
//     case 1:
//         motors.value(ui->tableWidget->verticalHeaderItem(row)->text())->setSpeedCalFactor2(ui->tableWidget->item(row,1)->text().toDouble());
//         break;
//     case 2:
//         motors.value(ui->tableWidget->verticalHeaderItem(row)->text())->setSpeedCalFactor3(ui->tableWidget->item(row,2)->text().toDouble());
//         break;
//     case 3:
//         motors.value(ui->tableWidget->verticalHeaderItem(row)->text())->setSpeedCalFactor4(ui->tableWidget->item(row,3)->text().toDouble());
//         break;
//     case 4:
//         motors.value(ui->tableWidget->verticalHeaderItem(row)->text())->setSpeedCalFactor5(ui->tableWidget->item(row,4)->text().toDouble());
//         break;
//     case 5:
//         motors.value(ui->tableWidget->verticalHeaderItem(row)->text())->setSpeedCalFactor6(ui->tableWidget->item(row,5)->text().toDouble());
//         break;
//     default:
//         break;
//     }
// }


void MotorFactors::on_buttonBox_clicked(QAbstractButton *button)
{
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor1(ui->tableWidget->item(i,0)->text().toDouble());
        motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor2(ui->tableWidget->item(i,1)->text().toDouble());
        motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor3(ui->tableWidget->item(i,2)->text().toDouble());
        motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor4(ui->tableWidget->item(i,3)->text().toDouble());
        motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor5(ui->tableWidget->item(i,4)->text().toDouble());
        motors.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeedCalFactor6(ui->tableWidget->item(i,5)->text().toDouble());
    }

}

