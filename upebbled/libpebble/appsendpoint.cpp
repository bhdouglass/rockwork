#include "appsendpoint.h"
#include "pebble.h"
#include "watchconnection.h"
#include "watchdatareader.h"

AppsEndpoint::AppsEndpoint(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_watchConnection(connection)
{
    m_watchConnection->registerEndpointHandler(WatchConnection::EndpointApplicationMessage, this, "handleMessage");
    m_watchConnection->registerEndpointHandler(WatchConnection::EndpointAppFetch, this, "handleAppFetchMessage");
}

void AppsEndpoint::handleMessage(const QByteArray &data)
{
    qDebug() << "AppsEndpoint:" << data.toHex();

    switch (data.at(0)) {
    case AppMessagePush:
        qDebug() << "Got push message";
//        handlePushMessage(data);
        break;
    case AppMessageAck:
        qDebug() << "Got ack message";
//        handleAckMessage(data, true);
        break;
    case AppMessageNack:
        qDebug() << "Got Nack message";
//        handleAckMessage(data, false);
        break;
    default:
        qWarning() << "Unknown application message type:" << int(data.at(0));
        break;
    }



}

void AppsEndpoint::handleAppFetchMessage(const QByteArray &data)
{
    qDebug() << "should fetch" << data.toHex();
    WatchDataReader reader(data);
    reader.skip(1);
    qDebug() << reader.readUuid();
}

void AppsEndpoint::handlePushMessage(const QByteArray &data)
{
    quint8 transaction;
    QUuid uuid;
    WatchConnection::Dict dict;

    if (!unpackPushMessage(data, &transaction, &uuid, &dict)) {
        qWarning() << "Failed to parse App push message";
        m_watchConnection->writeToPebble(WatchConnection::EndpointApplicationMessage,
                           buildNackMessage(transaction));
        return;
    }

    qDebug() << "Received appmsg PUSH from" << uuid << "with" << dict;

    QVariantMap msg = mapAppKeys(uuid, dict);
    qDebug() << "Mapped dict" << msg;

//    bool result;

//    MessageHandlerFunc handler = _handlers.value(uuid);
//    if (handler) {
//        result = handler(msg);
//    } else {
//        // No handler? Let's just send an ACK.
//        result = false;
//    }

//    if (result) {
//        qCDebug(l) << "ACKing transaction" << transaction;
//        watch->sendMessage(WatchConnector::watchAPPLICATION_MESSAGE,
//                           buildAckMessage(transaction));
//    } else {
//        qCDebug(l) << "NACKing transaction" << transaction;
//        watch->sendMessage(WatchConnector::watchAPPLICATION_MESSAGE,
//                           buildNackMessage(transaction));
//    }
}

bool AppsEndpoint::unpackPushMessage(const QByteArray &msg, quint8 *transaction, QUuid *uuid, WatchConnection::Dict *dict)
{
    WatchDataReader reader(msg);
    quint8 code = reader.read<quint8>();
//    Q_UNUSED(code);
    qDebug() << "message code" << code;
//    Q_ASSERT(code == WatchConnector::appmsgPUSH);

    *transaction = reader.read<quint8>();
    *uuid = reader.readUuid();
    *dict = reader.readDict();

    if (reader.bad()) {
        return false;
    }

    return true;
}

QByteArray AppsEndpoint::buildAckMessage(quint8 transaction)
{
    QByteArray ba(2, Qt::Uninitialized);
    ba[0] = AppMessageAck;
    ba[1] = transaction;
    return ba;
}

QByteArray AppsEndpoint::buildNackMessage(quint8 transaction)
{
    QByteArray ba(2, Qt::Uninitialized);
    ba[0] = AppMessageNack;
    ba[1] = transaction;
    return ba;
}

QVariantMap AppsEndpoint::mapAppKeys(const QUuid &uuid, const WatchConnection::Dict &dict)
{
//    AppInfo info = apps->info(uuid);
//    if (info.uuid() != uuid) {
//        qCWarning(l) << "Unknown app GUID while sending message:" << uuid;
//    }

    QVariantMap data;

//    for (WatchConnector::Dict::const_iterator it = dict.constBegin(); it != dict.constEnd(); ++it) {
//        if (info.hasAppKeyValue(it.key())) {
//            data.insert(info.appKeyForValue(it.key()), it.value());
//        } else {
//            qCWarning(l) << "Unknown appKey value" << it.key() << "for app with GUID" << uuid;
//            data.insert(QString::number(it.key()), it.value());
//        }
//    }

    return data;
}
