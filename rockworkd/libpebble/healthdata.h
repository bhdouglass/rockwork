#ifndef HEALTHDATA_H
#define HEALTHDATA_H

#include <QObject>
#include <QSqlDatabase>

#include "pebble.h"

class HealthData: public QObject
{
    Q_OBJECT
public:
    enum SleepType {
        SleepTypeNormal    = 0,
        SleepTypeDeepSleep = 1
    };

    HealthData(Pebble *pebble);

    int steps(const QDateTime &startTime, const QDateTime &endTime) const;
    int averageSteps(const QDateTime &startTime, const QDateTime &endTime) const;

    QVariantList sleepDataForDay(const QDate &day) const;
    int sleepAverage(const QDate &startDate, const QDate &endDate, SleepType type) const;
    QVariantMap averageSleepTimes(const QDate &day) const;

public slots:
    void addHealthData(const QByteArray &data);
    void addSleepData(const QByteArray &data);
private:
    void initdb();

    Pebble *m_pebble;
    QSqlDatabase m_db;
};

#endif // HEALTHDATA_H
