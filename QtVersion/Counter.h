//
// Created by jeffl on 1/19/2024.
//

#ifndef CONVEYORINTERFACEQT_COUNTER_H
#define CONVEYORINTERFACEQT_COUNTER_H

#include <QObject>
#include <QFile>


class Counter : public QObject
{
    Q_OBJECT

public:
    explicit Counter(QObject *parent = nullptr);
    explicit Counter(int totalCounter, QObject *parent = nullptr);

    int totalCounter() const;
    void setTotalCounter(int newTotalCounter);

    void readTotalCounter();
    void writeTotalCounter(int count);

private:
    int m_count {0};
    QString m_fileName {"counter.txt"};

public:

    int count() const;
    void setCount(int newCount);
    QString fileName() const;
    void setFileName(const QString &newFileName);
};


#endif //CONVEYORINTERFACEQT_COUNTER_H
