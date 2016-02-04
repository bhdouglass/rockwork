#include "watchlogendpoint.h"
#include "watchdatawriter.h"
#include "watchdatareader.h"
#include "pebble.h"

WatchLogEndpoint::WatchLogEndpoint(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_connection(connection)
{
    m_connection->registerEndpointHandler(WatchConnection::EndpointLogDump, this, "logMessageReceived");
}

void WatchLogEndpoint::fetchLogs()
{
    quint32 cookie = qrand();
    RequestLogPacket packet(WatchLogEndpoint::LogCommandRequestLogs, 0, cookie);
    qDebug() << "Dumping logs" << packet.serialize().toHex() << "cookie" << cookie;
    m_connection->writeToPebble(WatchConnection::EndpointLogDump, packet.serialize());
}

void WatchLogEndpoint::logMessageReceived(const QByteArray &data)
{
    WatchDataReader reader(data);
    quint8 command = reader.read<quint8>();
    switch (command) {
    case LogCommandLogMessage: {
        LogMessage m(data.right(data.length() - 1));
        qDebug() << m.filename() << ":" << m.line() << ":" << m.message();
        break;
    }
    default:
        qWarning() << "LogEndpoint: Unhandled command" << command;
    }
}


RequestLogPacket::RequestLogPacket(WatchLogEndpoint::LogCommand command, quint8 generation, quint32 cookie):
    m_command(command),
    m_generation(generation),
    m_cookie(cookie)
{

}

QByteArray RequestLogPacket::serialize() const
{
    QByteArray msg;
    WatchDataWriter writer(&msg);
    writer.write<quint8>(m_command);
    writer.write<quint8>(m_generation);
    writer.write<quint32>(m_cookie);
    return msg;
}

LogMessage::LogMessage(const QByteArray &data)
{
    WatchDataReader reader(data);
    m_cookie = reader.read<quint32>();
    m_timestamp = QDateTime::fromTime_t(reader.read<quint32>());
    m_level = reader.read<quint8>();
    m_length = reader.read<quint8>();
    m_line = reader.read<quint16>();
    m_filename = reader.readFixedString(16);
    m_message = reader.readFixedString(m_length);
}
