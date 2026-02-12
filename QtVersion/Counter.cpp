//
// Created by jeffl on 1/19/2024.
//
#include <QDebug>
#include "Counter.h"

Counter::Counter(QObject *parent)
{

}

void Counter::readTotalCounter()
{
    QFile file(m_fileName);
    if (file.open(QIODevice::ReadOnly)) {
        m_count = file.readAll().toInt();
        file.close();
        //ui->lcdTotalCounter->display(m_count); //move to mainwindow.cpp

        return;
    }
    else {
        qInfo() << "Could not open file " << m_fileName;
    }
}

void Counter::writeTotalCounter(int count)
{
    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QString::number(count).toUtf8());
        file.close();
    }
    else {
        qInfo() << "Could not open file " << m_fileName;
    }
}

QString Counter::fileName() const
{
    return m_fileName;
}

void Counter::setFileName(const QString &newFileName)
{
    m_fileName = newFileName;
}

int Counter::count() const
{
    return m_count;
}

void Counter::setCount(int newCount)
{
    m_count = newCount;
}

