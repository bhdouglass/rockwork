#include "healthdata.h"
#include "watchdatareader.h"

#include <QSettings>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>

HealthData::HealthData(Pebble *pebble):
    m_pebble(pebble),
    QObject(pebble)
{
    QString dbPath = pebble->storagePath();
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    QDir dir;
    dir.mkpath(dbPath);
    m_db.setDatabaseName(dbPath + "/healthdata.sqlite");
    initdb();
}

int HealthData::steps(const QDateTime &startTime, const QDateTime &endTime) const
{
    QString queryString = QString("SELECT SUM(steps) from healthdata WHERE timestamp >= %1 AND timestamp <= %2").arg(startTime.toMSecsSinceEpoch() / 1000).arg(endTime.toMSecsSinceEpoch() / 1000);
    QSqlQuery query;
    query.exec(queryString);
    qDebug() << "Query:" << queryString;
    if (query.next()) {
        return query.value(0).toInt();
    } else {
        qWarning() << "Error executing query:" << query.lastError().text() << "Query was:" << queryString;
    }
    return 0;
}

int HealthData::averageSteps(const QDateTime &startTime, const QDateTime &endTime) const
{
    int count = 0;
    int steps = 0;
    for (int i = 0; i < 30; i++) {
        QDateTime start = startTime.addDays(-i);
        QDateTime end = endTime.addDays(-i);
        QString queryString = QString("SELECT COUNT(*) from healthdata WHERE (timestamp >= %1 AND timestamp <= %2)").arg(start.toMSecsSinceEpoch() / 1000).arg(end.toMSecsSinceEpoch() / 1000);
        QSqlQuery query;
        query.exec(queryString);
        if (!query.next()) {
            continue;
        }
        if (query.value(0).toInt() == 0) {
            continue;
        }

        queryString = QString("SELECT SUM(steps) from healthdata WHERE (timestamp >= %1 AND timestamp <= %2)").arg(start.toMSecsSinceEpoch() / 1000).arg(end.toMSecsSinceEpoch() / 1000);
        query.exec(queryString);
        if (query.next()) {
            steps += query.value(0).toInt();
            count++;
        } else {
            qWarning() << "Error executing query:" << query.lastError().text() << "Query was:" << queryString;
        }
    }
    return count > 0 ? steps / count : 0;
}

QVariantList HealthData::sleepDataForDay(const QDate &day) const
{
    QTime cutTime(12, 0);
    QDateTime startDateTime(day.addDays(-1));
    startDateTime.setTime(cutTime);
    QDateTime endDateTime = startDateTime.addDays(1);
    QString queryString = QString("SELECT * from sleepdata WHERE starttime >= %1 AND starttime <= %2").arg(startDateTime.toMSecsSinceEpoch() / 1000).arg(endDateTime.toMSecsSinceEpoch() / 1000);
    QSqlQuery query;
    bool result = query.exec(queryString);
    if (!result) {
        qWarning() << "Error executing query:" << query.lastError().text() << "Query was:" << queryString;
    }
    QVariantList ret;
    while (query.next()) {
        QVariantMap entry;
        entry.insert("starttime", query.value(0).toLongLong());
        entry.insert("duration", query.value(1).toInt());
        entry.insert("type", query.value(2).toInt());
        ret.append(entry);
    }
    return ret;
}

int HealthData::sleepAverage(const QDate &startDate, const QDate &endDate, SleepType type) const
{
    QDate currentDay = startDate;

    int count = 0;
    int total = 0;
    while (currentDay <= endDate) {
        QVariantList dayData = sleepDataForDay(currentDay);
        bool haveData = false;
        for (int i = 0; i < dayData.count(); i++) {
            if ((SleepType)dayData.at(i).toMap().value("type").toInt() == type) {
                haveData = true;
                total += dayData.at(i).toMap().value("duration").toInt();
            }
        }
        if (haveData) {
            count++;
        }
        currentDay = currentDay.addDays(1);
    }
    if (count == 0) {
        return 0;
    }
    return total / count;
}

QVariantMap HealthData::averageSleepTimes(const QDate &day) const
{
    bool isWeekend = day.dayOfWeek() >= 6;
    int count = 0;
    quint64 averageFallAsleepTime = 0;
    quint64 averageWakeupTime = 0;
    quint64 averageSleepTime = 0;
    quint64 averageDeepSleep = 0;

    quint64 twentyFour = 24 * 60 * 60 * 1000;
    for (int i = 0; i < 30; i++) {
        if ((isWeekend && day.addDays(-i).dayOfWeek() < 6) || (!isWeekend && day.addDays(-i).dayOfWeek() >= 6)) {
            continue;
        }
        QVariantList dayData = sleepDataForDay(day.addDays(-i));
        if (dayData.isEmpty()) {
            continue;
        }
        QDateTime fallAsleepTime;
        QDateTime wakeupTime;
        for (int j = 0; j < dayData.count(); j++) {
            QVariantMap entry = dayData.at(j).toMap();
            if ((SleepType)entry.value("type").toInt() == SleepTypeNormal) {
                averageSleepTime += entry.value("duration").toInt();
                QDateTime entryFallAsleepTime = QDateTime::fromMSecsSinceEpoch(entry.value("starttime").toLongLong() * 1000);
                if (fallAsleepTime.isNull() || entryFallAsleepTime < fallAsleepTime) {
                    fallAsleepTime = entryFallAsleepTime;
                }
                QDateTime entryWakeupTime = QDateTime::fromMSecsSinceEpoch((entry.value("starttime").toLongLong() + entry.value("duration").toInt()) * 1000);
                if (wakeupTime.isNull() || entryWakeupTime > wakeupTime) {
                    wakeupTime = entryWakeupTime;
                }
            } else {
                averageDeepSleep += entry.value("duration").toInt();
            }
        }
        count++;
        averageFallAsleepTime += fallAsleepTime.time().msecsSinceStartOfDay();
        if (fallAsleepTime.time() < QTime(12,0,0)) {
            averageFallAsleepTime += twentyFour;
        }
        averageWakeupTime += wakeupTime.time().msecsSinceStartOfDay();
        if (wakeupTime.time() < QTime(18, 0, 0)) {
            averageWakeupTime += twentyFour;
        }

    }
    if (count == 0) {
        qWarning() << "No sleep records available for requested date:" << day;
        return QVariantMap();
    }
    QDateTime sleepDateTime;
    sleepDateTime.setDate(day);
    averageFallAsleepTime = averageFallAsleepTime / count;
    if (averageFallAsleepTime > twentyFour) {
        averageFallAsleepTime -= twentyFour;
        sleepDateTime = sleepDateTime.addDays(-1);
    }
    sleepDateTime.setTime(QTime::fromMSecsSinceStartOfDay(averageFallAsleepTime));

    QDateTime wakeupDateTime;
    wakeupDateTime.setDate(day);
    averageWakeupTime = averageWakeupTime / count;
    if (averageWakeupTime > twentyFour) {
        averageWakeupTime -= twentyFour;
        wakeupDateTime = wakeupDateTime.addDays(-1);
    }
    wakeupDateTime.setTime(QTime::fromMSecsSinceStartOfDay(averageWakeupTime));

    averageSleepTime /= count;
    averageDeepSleep /= count;

    QVariantMap ret;
    ret.insert("fallasleep", sleepDateTime.toMSecsSinceEpoch() / 1000);
    ret.insert("wakeup", wakeupDateTime.toMSecsSinceEpoch() / 1000);
    ret.insert("sleepTime", averageSleepTime);
    ret.insert("deepSleep", averageDeepSleep);
    return ret;
}

#include <QTimeZone>

void HealthData::addHealthData(const QByteArray &data)
{
    qDebug() << "Adding health data to log:" << data.toHex();

    WatchDataReader reader(data);
    while (reader.offset() < data.length() && !reader.bad()) {
        quint16 version = reader.readLE<quint16>();
        if (version != 6) {
            qDebug() << "Data version" << version << "is unsupported. Health data logging might not work.";
            return;
        }

        QDateTime timeStamp = reader.readTimestamp();
        int timeLocalOffset = reader.read<quint8>();
        qDebug() << "Raw timestamp:"  << timeStamp.toString() << "offset" << timeLocalOffset;
        timeStamp.setTimeSpec(Qt::UTC);
        timeStamp = timeStamp.addSecs(-timeLocalOffset * 15 * 60);
        qDebug() << "corrected timestamp" << timeStamp;
        int sampleSize = reader.read<quint8>();
        if (sampleSize != 12) {
            qWarning() << "Invalid sample size. Cannot parse health data. Got:" << sampleSize << "Expected:" << 15;
            return;
        }
        int samples = reader.read<quint8>();
        for (int i = 0; i < samples; i++) {
            QDateTime sampleTimestamp = timeStamp.addSecs(60 * i);
            quint8 steps = reader.read<quint8>();
            quint8 orientation = reader.read<quint8>();
            quint16 vmc = reader.readLE<quint16>();
            quint8 light = reader.read<quint8>();
            quint8 uk1 = reader.read<quint8>();
            quint8 uk2 = reader.read<quint8>();
            quint8 uk3 = reader.read<quint8>();
            quint8 uk4 = reader.read<quint8>();
            quint8 uk5 = reader.read<quint8>();
            quint8 uk6 = reader.read<quint8>();
            quint8 uk7 = reader.read<quint8>();

            QSqlQuery query;
            QString queryString = QString("INSERT INTO healthdata (timestamp, steps, orientation, vmc, light, uk1, uk2, uk3, uk4, uk5, uk6, uk7) VALUES (%1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12)")
                    .arg(sampleTimestamp.toMSecsSinceEpoch() / 1000)
                    .arg(steps)
                    .arg(orientation)
                    .arg(vmc)
                    .arg(light)
                    .arg(uk1)
                    .arg(uk2)
                    .arg(uk3)
                    .arg(uk4)
                    .arg(uk5)
                    .arg(uk6)
                    .arg(uk7);
            bool result = query.exec(queryString);

            qDebug()<< "inserting timestamp" << sampleTimestamp.toString();
            if (!result) {
                qWarning() << "Error inserting health data:" << query.lastError().text();
                qDebug() << "Query was:" << queryString;
            }

        }
    }
    if (reader.offset() < data.length()) {
        qWarning() << "########################################################## mismatch" << reader.offset() << data.length();
    }
}

void HealthData::addSleepData(const QByteArray &data)
{
    WatchDataReader reader(data);

    while (reader.offset() < data.length()) {
        //        qDebug() << data.right(data.length() - reader.offset()).left(20).toHex();
        // I've really no clue yet what the vales exactly mean
        quint8 uk1 = reader.read<quint8>();
        quint8 uk2 = reader.read<quint8>();
        quint8 uk3 = reader.read<quint8>();
        quint8 uk4 = reader.read<quint8>();
        QDateTime timestamp = reader.readTimestamp();

        quint8 uk5 = reader.read<quint8>();
        quint8 uk6 = reader.read<quint8>();

        if ((uk5 == 1 || uk5 == 2) && uk6 == 0) {
            QDateTime sleepTime = reader.readTimestamp();
            quint16 duration = reader.readLE<quint16>();
            reader.skip(20); // Seems to be all 0
            QSqlQuery query;
            bool result = query.exec(QString("INSERT INTO sleepdata (starttime, duration, type) VALUES (%1, %2, %3)")
                                     .arg(sleepTime.toMSecsSinceEpoch() / 1000)
                                     .arg(duration)
                                     .arg(uk5 == 2 ? SleepTypeDeepSleep : SleepTypeNormal));
            if (!result) {
                qDebug() << "Error inserting sleep data:" << query.lastError().text();
            }
        } else {
            qDebug() << "Unknown sleep record";
            reader.skip(26);
        }
    }
}

void HealthData::initdb()
{
    m_db.open();
    if (!m_db.open()) {
        qWarning() << "Error opening state database:" << m_db.lastError().driverText() << m_db.lastError().databaseText();
        return;
    }

    if (!m_db.tables().contains(QStringLiteral("healthdata"))) {
        QSqlQuery query;
        query.exec(QString("CREATE TABLE healthdata( \
                           timestamp INTEGER UNIQUE, \
                           steps INTEGER, \
                           orientation INTEGER, \
                           vmc INTEGER, \
                           light INTEGER, \
                           uk1 INTEGER, \
                           uk2 INTEGER, \
                           uk3 INTEGER, \
                           uk4 INTEGER, \
                           uk5 INTEGER, \
                           uk6 INTEGER, \
                           uk7 INTEGER \
                           );"));
        // For testing...
        QFile f(m_pebble->storagePath() + "/datalog45.log");
        f.open(QFile::ReadOnly);
        while (!f.atEnd()) {
            QByteArray line = QByteArray::fromHex(f.readLine());
            addHealthData(line);
        }

        // For testing...
        QFile f1(m_pebble->storagePath() + "/datalog217.log");
        f1.open(QFile::ReadOnly);
        while (!f1.atEnd()) {
            QByteArray line = QByteArray::fromHex(f1.readLine());
            addHealthData(line);
        }

        // For testing...
        QFile f2(m_pebble->storagePath() + "/datalog61.log");
        f2.open(QFile::ReadOnly);
        while (!f2.atEnd()) {
            QByteArray line = QByteArray::fromHex(f2.readLine());
            addHealthData(line);
        }
        // For testing...
        QFile f3(m_pebble->storagePath() + "/datalog158.log");
        f3.open(QFile::ReadOnly);
        while (!f3.atEnd()) {
            QByteArray line = QByteArray::fromHex(f3.readLine());
            addHealthData(line);
        }
        // For testing...
        QFile f4(m_pebble->storagePath() + "/datalog40.log");
        f4.open(QFile::ReadOnly);
        while (!f4.atEnd()) {
            QByteArray line = QByteArray::fromHex(f4.readLine());
            addHealthData(line);
        }
    }

    if (!m_db.tables().contains(QStringLiteral("sleepdata"))) {
        QSqlQuery query;
        int result = query.exec(QString("CREATE TABLE sleepdata( \
                                        starttime INTEGER, \
                                        duration INTEGER, \
                                        type INTEGER \
                                        );"));
        if (!result) {
            qDebug() << "Error creating sleepdata table:" << query.lastError().text();
        }

        // For testing...
        QFile f(m_pebble->storagePath() + "/datalog160.log");
        f.open(QFile::ReadOnly);
        while (!f.atEnd()) {
            QByteArray line = QByteArray::fromHex(f.readLine());
            addSleepData(line);
        }
        // For testing...
        QFile f1(m_pebble->storagePath() + "/datalog20.log");
        f1.open(QFile::ReadOnly);
        while (!f1.atEnd()) {
            QByteArray line = QByteArray::fromHex(f1.readLine());
            addSleepData(line);
        }

    }
}

