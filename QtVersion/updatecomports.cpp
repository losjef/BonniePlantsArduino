#include "updatecomports.h"
#include "ui_updatecomports.h"

UpdateCOMPorts::UpdateCOMPorts(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpdateCOMPorts)
{
    ui->setupUi(this);
}

UpdateCOMPorts::~UpdateCOMPorts()
{
    delete ui;
}

void UpdateCOMPorts::on_buttonBox_accepted()
{
    emit sendCOMPorts(ui->lineEditInputCOM->text(), ui->lineEditOutputCOM->text());
}


void UpdateCOMPorts::fillCOMPorts(QString inputComPort, QString outputComPort) {
    ui->lineEditOutputCOM->setText(outputComPort);
    ui->lineEditInputCOM->setText(inputComPort);
}

