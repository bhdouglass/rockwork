#ifndef APPSENDPOINT_H
#define APPSENDPOINT_H

#include "watchconnection.h"

#include <QObject>

class Pebble;

class AppsEndpoint : public QObject
{
    Q_OBJECT
public:
    enum AppMessage {
        AppMessagePush = 1,
        AppMessageRequest = 2,
        AppMessageAck = 0xFF,
        AppMessageNack = 0x7F
    };

    explicit AppsEndpoint(Pebble *pebble, WatchConnection *connection);

private slots:
    void handleMessage(const QByteArray &data);
    void handleAppFetchMessage(const QByteArray &data);

private:
    void handlePushMessage(const QByteArray &data);
    bool unpackPushMessage(const QByteArray &msg, quint8 *transaction, QUuid *uuid, WatchConnection::Dict *dict);

    QByteArray buildAckMessage(quint8 transaction);
    QByteArray buildNackMessage(quint8 transaction);
    QVariantMap mapAppKeys(const QUuid &uuid, const WatchConnection::Dict &dict);

private:
    Pebble *m_pebble;
    WatchConnection *m_watchConnection;
};

#endif // APPSENDPOINT_H
