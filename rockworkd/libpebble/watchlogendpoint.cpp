#include "watchlogendpoint.h"
#include "watchdatawriter.h"
#include "watchdatareader.h"
#include "pebble.h"
#include "ziphelper.h"

#include <QDir>

WatchLogEndpoint::WatchLogEndpoint(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_connection(connection)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    m_connection->registerEndpointHandler(WatchConnection::EndpointLogDump, this, "logMessageReceived");
}

void WatchLogEndpoint::fetchLogs(const QString &targetArchive)
{
    if (m_currentEpoch != 0) {
        qWarning() << "Already dumping logs. Not starting a second time";
        return;
    }
    m_targetArchive = targetArchive;
    fetchForEpoch(m_currentEpoch);
}

void WatchLogEndpoint::fetchForEpoch(quint8 epoch)
{
    QDir dir(m_pebble->storagePath() + "/watchlogs/");
    if (!dir.exists() && !dir.mkpath(dir.absolutePath())) {
        qWarning() << "Error creating log dir";
        emit logsFetched(false);
        return;
    }

    m_currentFile.setFileName(dir.absolutePath() + "/logdump_epoch" + QString::number(epoch) + ".log");
    if (!m_currentFile.open(QFile::WriteOnly)) {
        qWarning() << "Cannot open log file for writing" << m_currentFile.fileName();
        return;
    }
    qDebug() << "Dumping logs for epoch" << epoch;
    RequestLogPacket packet(WatchLogEndpoint::LogCommandRequestLogs, epoch, qrand());
    m_connection->writeToPebble(WatchConnection::EndpointLogDump, packet.serialize());
}

void WatchLogEndpoint::logMessageReceived(const QByteArray &data)
{
    WatchDataReader reader(data);
    quint8 command = reader.read<quint8>();
    switch (command) {
    case LogCommandLogMessage: {
        LogMessage m(data.right(data.length() - 1));
        QString line("%1 %2 :%3> %4\n");
        line = line.arg(m.level()).arg(m.timestamp().toString("yyyy-MM-dd hh:mm:ss")).arg(m.line()).arg(m.message());
        m_currentFile.write(line.toUtf8());
        break;
    }
    case LogCommandLogMessageDone: {
        qDebug() << "Log for epoch" << m_currentEpoch << "fetched";
        m_currentFile.close();
        m_currentEpoch++;
        if (m_currentEpoch == 0) {
            // My watch doesn't ever seem to give me LogCommandNoLogMessages. Make sure we don't cycle endlessly
            qDebug() << "All 255 epocs fetched. Stopping";
            packLogs();
            return;
        }
        fetchForEpoch(m_currentEpoch);
        break;
    }
    case LogCommandNoLogMessages:
        qDebug() << "Log dumping finished";
        m_currentEpoch = 0;
        packLogs();
        break;
    default:
        qWarning() << "LogEndpoint: Unhandled command" << command;
    }
}

void WatchLogEndpoint::packLogs()
{
    QString source = m_pebble->storagePath() + "/watchlogs/";
    bool success = ZipHelper::packArchive(m_targetArchive, source);
    if (success) {
        qDebug() << "Logs packed to" << m_targetArchive;
    } else {
        qWarning() << "Error packing logs from" << source << "to" << m_targetArchive;
    }
    emit logsFetched(success);
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
    int level = reader.read<quint8>();
    switch (level) {
    case 0:
        m_level = '*';
        break;
    case 1:
        m_level = 'E';
        break;
    case 50:
        m_level = 'W';
        break;
    case 100:
        m_level = 'I';
        break;
    case 200:
        m_level = 'D';
    case 250:
        m_level = 'V';
    }

    m_length = reader.read<quint8>();
    m_line = reader.read<quint16>();
    m_filename = reader.readFixedString(16);
    m_message = reader.readFixedString(m_length);
}
