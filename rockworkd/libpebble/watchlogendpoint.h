#ifndef WATCHLOGENDPOINT_H
#define WATCHLOGENDPOINT_H

#include <QObject>
#include <QDateTime>

#include "watchconnection.h"

class Pebble;

class LogMessage: public PebblePacket
{
public:
    LogMessage(const QByteArray &data);

    quint32 cookie() const { return m_cookie; }
    QDateTime timestamp() const { return m_timestamp; }
    quint8 level() const { return m_level; }
    quint8 length() const { return m_length; }
    quint16 line() const { return m_line; }
    QString filename() const { return m_filename; }
    QString message() const { return m_message; }

    QByteArray serialize() const override { return QByteArray(); }
private:
    quint32 m_cookie;
    QDateTime m_timestamp;
    quint8 m_level;
    quint8 m_length;
    quint16 m_line;
    QString m_filename;
    QString m_message;
};

class WatchLogEndpoint : public QObject
{
    Q_OBJECT
public:
    enum LogCommand {
        LogCommandRequestLogs = 0x10,
        LogCommandLogMessage = 0x80,
        LogCommandLogMessageDone = 0x81,
        LogCommandNoLogMessages = 0x82
    };

    explicit WatchLogEndpoint(Pebble *pebble, WatchConnection *connection);

    void fetchLogs();
signals:

private slots:
    void logMessageReceived(const QByteArray &data);

private:
    WatchConnection *m_connection;
};

class RequestLogPacket: public PebblePacket
{
public:
    RequestLogPacket(WatchLogEndpoint::LogCommand command, quint8 generation, quint32 cookie);
    QByteArray serialize() const;
private:
    WatchLogEndpoint::LogCommand m_command;
    quint8 m_generation;
    quint32 m_cookie;
};

#endif // WATCHLOGENDPOINT_H
