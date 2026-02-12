#ifndef UPPERSOILBELTFACT_H
#define UPPERSOILBELTFACT_H

#include <QDialog>
#include "tray.h"
#include "qabstractbutton.h"

namespace Ui {
class UpperSoilBeltFact;
}

class UpperSoilBeltFact : public QDialog
{
    Q_OBJECT

public:
    explicit UpperSoilBeltFact(QHash<QString, QSharedPointer<Tray>> trays, QWidget *parent = nullptr);
    ~UpperSoilBeltFact();

    void getTrayMotor8Factors();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::UpperSoilBeltFact *ui;
    QHash<QString, QSharedPointer<Tray>> trays;
};

#endif // UPPERSOILBELTFACT_H
