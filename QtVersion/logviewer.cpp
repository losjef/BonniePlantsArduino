#include "logviewer.h"
#include "ui_logviewer.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

LogViewer::LogViewer(ProductionLog* productionLog, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogViewer),
    m_productionLog(productionLog)
{
    ui->setupUi(this);
    
    // Configure table
    ui->tableWidget->setColumnCount(5);
    QStringList headers;
    headers << "Date/Time" << "Count" << "Speed" << "Tray" << "Total Counter";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    
    // Set column widths
    ui->tableWidget->setColumnWidth(0, 150);  // Date/Time
    ui->tableWidget->setColumnWidth(1, 80);   // Count
    ui->tableWidget->setColumnWidth(2, 60);   // Speed
    ui->tableWidget->setColumnWidth(3, 100);  // Tray
    
    loadLogData();
    updateStatistics();
}

LogViewer::~LogViewer()
{
    delete ui;
}

void LogViewer::loadLogData()
{
    auto entries = m_productionLog->getAllEntries();
    
    ui->tableWidget->setRowCount(entries.size());
    
    // Load in reverse order (newest first)
    for (int i = 0; i < entries.size(); ++i) {
        int row = entries.size() - 1 - i;  // Reverse order
        const auto& entry = entries[i];
        
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(entry.timestamp.toString("yyyy-MM-dd HH:mm:ss")));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(entry.count)));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(entry.speedSetting)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(entry.trayName));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(entry.totalCounter)));
    }
    
    ui->labelTotalEntries->setText(QString("Total Entries: %1").arg(entries.size()));
}

void LogViewer::updateStatistics()
{
    auto entries = m_productionLog->getAllEntries();
    
    if (entries.isEmpty()) {
        ui->labelStats->setText("No production runs logged yet");
        return;
    }
    
    // Calculate statistics
    int totalCount = 0;
    for (const auto& entry : entries) {
        totalCount += entry.count;
    }
    
    int avgCount = entries.isEmpty() ? 0 : totalCount / entries.size();
    
    QDateTime oldest = entries.first().timestamp;
    QDateTime newest = entries.last().timestamp;
    
    QString stats = QString("Average Count: %1 | Date Range: %2 to %3")
        .arg(avgCount)
        .arg(oldest.toString("yyyy-MM-dd"))
        .arg(newest.toString("yyyy-MM-dd"));
    
    ui->labelStats->setText(stats);
}

void LogViewer::on_pushButtonRefresh_clicked()
{
    loadLogData();
    updateStatistics();
    QMessageBox::information(this, "Refresh", "Log data refreshed");
}

void LogViewer::on_pushButtonExport_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
        "Export Production Log",
        "ProductionLog_Export.csv",
        "CSV Files (*.csv)");
    
    if (!filename.isEmpty()) {
        if (m_productionLog->exportToCSV(filename)) {
            QMessageBox::information(this, "Export Successful",
                QString("Production log exported to:\n%1").arg(filename));
        } else {
            QMessageBox::warning(this, "Export Failed",
                "Failed to export production log. Check file permissions.");
        }
    }
}

void LogViewer::on_pushButtonClear_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Clear Production Log",
        "Are you sure you want to clear all production log entries?\n\nThis action cannot be undone!",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_productionLog->clearLogs();
        loadLogData();
        updateStatistics();
        QMessageBox::information(this, "Cleared", "Production log has been cleared");
    }
}

void LogViewer::on_pushButtonClose_clicked()
{
    accept();
}
