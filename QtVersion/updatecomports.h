#ifndef UPDATECOMPORTS_H
#define UPDATECOMPORTS_H

#include <QDialog>

namespace Ui {
class UpdateCOMPorts;
}

class UpdateCOMPorts : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateCOMPorts(QWidget *parent = nullptr);
    ~UpdateCOMPorts();

    void fillCOMPorts(QString inputComPort, QString outputComPort);

private slots:
    void on_buttonBox_accepted();

signals:
    void sendCOMPorts(QString inputComPort, QString outputComPort);

private:
    Ui::UpdateCOMPorts *ui;
};

#endif // UPDATECOMPORTS_H
