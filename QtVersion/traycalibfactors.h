#ifndef TRAYCALIBFACTORS_H
#define TRAYCALIBFACTORS_H

#include <QDialog>
#include "tray.h"

namespace Ui {
class TrayCalibFactors;
}

class TrayCalibFactors : public QDialog
{
    Q_OBJECT

public:
    explicit TrayCalibFactors(QHash<QString, QSharedPointer<Tray>> trays, QWidget *parent = nullptr);
    ~TrayCalibFactors();

    void getTrayFactors();

private:
    Ui::TrayCalibFactors *ui;

    QHash<QString, QSharedPointer<Tray>> trays;

private slots:
    void on_buttonBox_accepted();
};

#endif // TRAYCALIBFACTORS_H
