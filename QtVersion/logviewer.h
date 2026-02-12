#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include "ProductionLog.h"

namespace Ui {
class LogViewer;
}

class LogViewer : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewer(ProductionLog* productionLog, QWidget *parent = nullptr);
    ~LogViewer();

private slots:
    void on_pushButtonRefresh_clicked();
    void on_pushButtonExport_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonClose_clicked();

private:
    Ui::LogViewer *ui;
    ProductionLog* m_productionLog;
    
    void loadLogData();
    void updateStatistics();
};

#endif // LOGVIEWER_H
