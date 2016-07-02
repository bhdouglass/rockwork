#ifndef DATALOGGINGENDPOINT_H
#define DATALOGGINGENDPOINT_H

#include <QObject>

class Pebble;
class WatchConnection;

class DataLoggingEndpoint : public QObject
{
    Q_OBJECT
public:
    enum DataLoggingCommand {
        DataLoggingDespoolOpenSession = 0x01,
        DataLoggingDespoolSendData = 0x02,
        DataLoggingCloseSession = 0x03,
        DataLoggingReportOpenSessions = 0x84,
        DataLoggingACK = 0x85,
        DataLoggingNACK = 0x86,
        DataLoggingTimeout = 0x07,
        DataLoggingEmptySession = 0x88,
        DataLoggingGetSendEnableRequest = 0x89,
        DataLoggingGetSendEnableResponse = 0x0A,
        DataLoggingSetSendEnable = 0x8B
    };

    enum LoggingTag {
        LoggingTagUnknown1 = 0x0000004E,
        LoggingTagUnknown2 = 0x0000004F,
        LoggingTagSleep    = 0x00000050,
        LoggingTagHealth   = 0x00000051,
        LoggingTagUnknown3 = 0x00000052,
        LoggingTagUnknown4 = 0x00000053,
        LoggingTagUnknown5 = 0x00000054,
    };

    explicit DataLoggingEndpoint(Pebble *pebble, WatchConnection *connection);

signals:
    void healthDataLogged(const QByteArray &data);
    void sleepDataLogged(const QByteArray &data);

private slots:
    void handleMessage(const QByteArray &data);

private:
    Pebble *m_pebble;
    WatchConnection *m_connection;
};

#endif // DATALOGGINGENDPOINT_H
