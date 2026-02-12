#ifndef PRODUCTIONLOG_H
#define PRODUCTIONLOG_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>

struct ProductionLogEntry {
    QDateTime timestamp;
    int count;
    int speedSetting;
    QString trayName;
    int totalCounter;
    
    // Constructor
    ProductionLogEntry(QDateTime time = QDateTime::currentDateTime(),
                      int cnt = 0,
                      int speed = 0,
                      QString tray = "",
                      int total = 0)
        : timestamp(time), count(cnt), speedSetting(speed), 
          trayName(tray), totalCounter(total) {}
    
    // Convert to CSV line
    QString toCSV() const {
        return QString("%1,%2,%3,%4,%5")
            .arg(timestamp.toString("yyyy-MM-dd HH:mm:ss"))
            .arg(count)
            .arg(speedSetting)
            .arg(trayName)
            .arg(totalCounter);
    }
    
    // Parse from CSV line
    static ProductionLogEntry fromCSV(const QString& csvLine) {
        QStringList parts = csvLine.split(',');
        if (parts.size() >= 5) {
            return ProductionLogEntry(
                QDateTime::fromString(parts[0], "yyyy-MM-dd HH:mm:ss"),
                parts[1].toInt(),
                parts[2].toInt(),
                parts[3],
                parts[4].toInt()
            );
        }
        return ProductionLogEntry();
    }
};

class ProductionLog : public QObject
{
    Q_OBJECT

public:
    explicit ProductionLog(QObject *parent = nullptr);
    
    // Add a log entry
    void addEntry(int count, int speedSetting, const QString& trayName, int totalCounter);
    
    // Get all log entries
    QVector<ProductionLogEntry> getAllEntries() const;
    
    // Get recent entries (last N)
    QVector<ProductionLogEntry> getRecentEntries(int count) const;
    
    // Clear all logs
    void clearLogs();
    
    // Export to CSV
    bool exportToCSV(const QString& filename);
    
    // Get log file path
    QString getLogFilePath() const;

private:
    QString m_logFileName;
    QVector<ProductionLogEntry> m_entries;
    
    // Load logs from file
    void loadFromFile();
    
    // Save logs to file
    void saveToFile();
    
    // Ensure log file exists with header
    void ensureLogFileExists();
};

#endif // PRODUCTIONLOG_H
