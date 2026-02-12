#include "traycalibfactors.h"
#include "ui_traycalibfactors.h"

TrayCalibFactors::TrayCalibFactors(QHash<QString, QSharedPointer<Tray>> newTrays, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TrayCalibFactors)
{
    ui->setupUi(this);
    this->trays = newTrays;
}

TrayCalibFactors::~TrayCalibFactors()
{
    delete ui;
}

void TrayCalibFactors::getTrayFactors()
{
    QHashIterator<QString, QSharedPointer<Tray>> i(trays);
    while(i.hasNext())
    {
        i.next();
        for(int j = 0; j < ui->tableWidget->rowCount(); j++)
        {
            if(ui->tableWidget->verticalHeaderItem(j)->text() == i.key())
            {
                QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i.value()->getSpeed1TimeFactor()));
                ui->tableWidget->setItem(j,0, item1);
                QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(i.value()->getSpeed2TimeFactor()));
                ui->tableWidget->setItem(j,1, item2);
                QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(i.value()->getSpeed3TimeFactor()));
                ui->tableWidget->setItem(j,2, item3);
                QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(i.value()->getSpeed4TimeFactor()));
                ui->tableWidget->setItem(j,3, item4);
                QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(i.value()->getSpeed5TimeFactor()));
                ui->tableWidget->setItem(j,4, item5);
                QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(i.value()->getSpeed6TimeFactor()));
                ui->tableWidget->setItem(j,5, item6);
            }
        }
    }
}

void TrayCalibFactors::on_buttonBox_accepted()
{
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeed1TimeFactor(ui->tableWidget->item(i,0)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeed2TimeFactor(ui->tableWidget->item(i,1)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeed3TimeFactor(ui->tableWidget->item(i,2)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeed4TimeFactor(ui->tableWidget->item(i,3)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeed5TimeFactor(ui->tableWidget->item(i,4)->text().toDouble());
        trays.value(ui->tableWidget->verticalHeaderItem(i)->text())->setSpeed6TimeFactor(ui->tableWidget->item(i,5)->text().toDouble());
    }
}
