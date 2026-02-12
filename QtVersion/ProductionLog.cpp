#include "ProductionLog.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

ProductionLog::ProductionLog(QObject *parent)
    : QObject(parent), m_logFileName("ProductionLog.csv")
{
    ensureLogFileExists();
    loadFromFile();
}

void ProductionLog::addEntry(int count, int speedSetting, const QString& trayName, int totalCounter)
{
    ProductionLogEntry entry(QDateTime::currentDateTime(), count, speedSetting, trayName, totalCounter);
    m_entries.append(entry);
    
    // Append to file immediately for data safety
    QFile file(m_logFileName);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << entry.toCSV() << "\n";
        file.close();
        qInfo() << "Production run logged: Count=" << count 
                << "Speed=" << speedSetting 
                << "Tray=" << trayName
                << "Total=" << totalCounter;
    } else {
        qWarning() << "Failed to write to production log file:" << file.errorString();
    }
}

QVector<ProductionLogEntry> ProductionLog::getAllEntries() const
{
    return m_entries;
}

QVector<ProductionLogEntry> ProductionLog::getRecentEntries(int count) const
{
    if (count >= m_entries.size()) {
        return m_entries;
    }
    
    QVector<ProductionLogEntry> recent;
    int startIndex = m_entries.size() - count;
    for (int i = startIndex; i < m_entries.size(); ++i) {
        recent.append(m_entries[i]);
    }
    return recent;
}

void ProductionLog::clearLogs()
{
    m_entries.clear();
    
    // Recreate file with just header
    QFile file(m_logFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Timestamp,Count,Speed,Tray,TotalCounter\n";
        file.close();
        qInfo() << "Production log cleared";
    }
}

bool ProductionLog::exportToCSV(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to export log to" << filename << ":" << file.errorString();
        return false;
    }
    
    QTextStream out(&file);
    out << "Timestamp,Count,Speed,Tray,TotalCounter\n";
    
    for (const auto& entry : m_entries) {
        out << entry.toCSV() << "\n";
    }
    
    file.close();
    qInfo() << "Production log exported to" << filename;
    return true;
}

QString ProductionLog::getLogFilePath() const
{
    return QDir::currentPath() + "/" + m_logFileName;
}

void ProductionLog::loadFromFile()
{
    QFile file(m_logFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo() << "No existing production log file found, will create new one";
        return;
    }
    
    QTextStream in(&file);
    QString header = in.readLine(); // Skip header
    
    m_entries.clear();
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            ProductionLogEntry entry = ProductionLogEntry::fromCSV(line);
            m_entries.append(entry);
        }
    }
    
    file.close();
    qInfo() << "Loaded" << m_entries.size() << "production log entries";
}

void ProductionLog::saveToFile()
{
    QFile file(m_logFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to save production log:" << file.errorString();
        return;
    }
    
    QTextStream out(&file);
    out << "Timestamp,Count,Speed,Tray,TotalCounter\n";
    
    for (const auto& entry : m_entries) {
        out << entry.toCSV() << "\n";
    }
    
    file.close();
}

void ProductionLog::ensureLogFileExists()
{
    QFile file(m_logFileName);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "Timestamp,Count,Speed,Tray,TotalCounter\n";
            file.close();
            qInfo() << "Created new production log file:" << m_logFileName;
        }
    }
}
