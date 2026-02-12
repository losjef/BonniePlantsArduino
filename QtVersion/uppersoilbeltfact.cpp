#include "uppersoilbeltfact.h"
#include "ui_uppersoilbeltfact.h"

UpperSoilBeltFact::UpperSoilBeltFact(QHash<QString, QSharedPointer<Tray> > newTrays, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpperSoilBeltFact)
{
    ui->setupUi(this);
    this->trays = newTrays;
    ui->tableWidget->setColumnCount(6);
}

UpperSoilBeltFact::~UpperSoilBeltFact()
{
    delete ui;
}

void UpperSoilBeltFact::getTrayMotor8Factors()
{
    QHashIterator i(trays);
    while(i.hasNext())
    {
        i.next();
        for(int j = 0; j < ui->tableWidget->rowCount(); j++)
        {
            if(ui->tableWidget->verticalHeaderItem(j)->text() == i.key())
            {
                QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i.value()->motor8SpeedCalFactor1()));
                ui->tableWidget->setItem(j,0, item1);
                QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(i.value()->motor8SpeedCalFactor2()));
                ui->tableWidget->setItem(j,1, item2);
                QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(i.value()->motor8SpeedCalFactor3()));
                ui->tableWidget->setItem(j,2, item3);
                QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(i.value()->motor8SpeedCalFactor4()));
                ui->tableWidget->setItem(j,3, item4);
                QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(i.value()->motor8SpeedCalFactor5()));
                ui->tableWidget->setItem(j,4, item5);
                QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(i.value()->motor8SpeedCalFactor6()));
                ui->tableWidget->setItem(j,5, item6);
            }
        }
    }
}

void UpperSoilBeltFact::on_buttonBox_clicked(QAbstractButton *button)
{
    // TODO: copied from motor factors
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setMotor8SpeedCalFactor1(ui->tableWidget->item(i,0)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setMotor8SpeedCalFactor2(ui->tableWidget->item(i,1)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setMotor8SpeedCalFactor3(ui->tableWidget->item(i,2)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setMotor8SpeedCalFactor4(ui->tableWidget->item(i,3)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setMotor8SpeedCalFactor5(ui->tableWidget->item(i,4)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setMotor8SpeedCalFactor6(ui->tableWidget->item(i,5)->text().toDouble());
    }
}

