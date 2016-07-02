#include "dataloggingendpoint.h"

#include "pebble.h"
#include "watchconnection.h"
#include "watchdatareader.h"
#include "watchdatawriter.h"

#include <QSettings>

DataLoggingEndpoint::DataLoggingEndpoint(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_connection(connection)
{
    m_connection->registerEndpointHandler(WatchConnection::EndpointDataLogging, this, "handleMessage");
}

void DataLoggingEndpoint::handleMessage(const QByteArray &data)
{
    qDebug() << "data logged" << data.toHex();
    WatchDataReader reader(data);

    QSettings settings(m_pebble->storagePath() + "/loggingsessions.conf", QSettings::IniFormat);

    DataLoggingCommand command = (DataLoggingCommand)reader.read<quint8>();
    quint8 sessionId = reader.read<quint8>();

    DataLoggingCommand returnCode = DataLoggingACK;

    switch (command) {
    case DataLoggingDespoolOpenSession: {
        QUuid appUuid = reader.readUuid();
        QDateTime timestamp = reader.readTimestamp();
        quint32 loggingTag = reader.readLE<quint32>();
        quint8 itemType = reader.read<quint8>();
        quint16 itemSize = reader.readLE<quint16>();

        qDebug() << "Starting new datalogging session for session" << sessionId << "Tag is:" << loggingTag;
        settings.setValue(QString::number(sessionId), loggingTag);

        QByteArray reply;
        WatchDataWriter writer(&reply);
        writer.write<quint8>(DataLoggingACK);
        writer.write<quint8>(sessionId);
        m_connection->writeToPebble(WatchConnection::EndpointDataLogging, reply);
        break;
    }
    case DataLoggingDespoolSendData: {
        quint32 itemsLeft = reader.readLE<quint32>();
        quint32 crc = reader.readLE<quint32>();
        qDebug() << "Despooling data: Session:" << sessionId << "Items left:" << itemsLeft << "CRC:" << crc;

        QFile f(m_pebble->storagePath() + "/datalog" + QString::number(sessionId) + ".log");
        f.open(QFile::WriteOnly | QFile::Append);
        f.write(data.right(data.length() - 10).toHex());
        f.write("\n");
        f.close();

        QByteArray sessionData = data.right(data.length() - 10);

        int loggingTag = settings.value(QString::number(sessionId), -1).toInt();

        switch (loggingTag) {
        case LoggingTagHealth:
            emit healthDataLogged(sessionData);
            break;
        case LoggingTagSleep:
            emit sleepDataLogged(sessionData);
            break;
        case LoggingTagUnknown1:
        case LoggingTagUnknown2:
        case LoggingTagUnknown3:
        case LoggingTagUnknown4:
        case LoggingTagUnknown5:
            // Don't do anything with them... just take them from the watch and ACK them to prevent
            // the watch from filling its memory
            break;
        default:
            // In case we received something where we can't find a matching tag,
            // assume we missed the DespoolOpenSession for it. Let's NACK it so
            // the watch will reopen the session for it and retransmit it.

            // IMPORTANT: If Pebble adds new logging data we'll continuously trigger this
            // which might cause watch instability after a long period of time.
            // This should be checked after every watch firmware upgrade and new logging data
            // end points should be added to the above case.

            // Even though this might become unstable in the future, this seems to be the
            // only way to reliably track data as the DespoolOpenSession can get lost...
            returnCode = DataLoggingNACK;
            qWarning() << "Unhandled logging data for session" << sessionId << "Tag:" << settings.value(QString::number(sessionId), -1);
        }

        break;
    }
    case DataLoggingTimeout: {
        qDebug() << "DataLogging reached timeout: Session:" << sessionId;
        break;
    }
    case DataLoggingCloseSession:
        qDebug() << "DataLogging session closed" << sessionId << "Tag:" << settings.value(QString::number(sessionId), -1);
        break;
    default:
        qWarning() << "Unhandled DataLogging message" << sessionId << "Tag:" << settings.value(QString::number(sessionId, -1));
        // Don't do anything in this case...
        return;
    }

    QByteArray reply;
    WatchDataWriter writer(&reply);
    writer.write<quint8>(returnCode);
    writer.write<quint8>(sessionId);
    m_connection->writeToPebble(WatchConnection::EndpointDataLogging, reply);
}

