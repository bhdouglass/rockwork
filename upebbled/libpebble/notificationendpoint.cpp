#include "notificationendpoint.h"

#include "watchconnection.h"
#include "pebble.h"
#include "blobdb.h"

#include <QDebug>
#include <QDateTime>

NotificationEndpoint::NotificationEndpoint(Pebble *pebble, WatchConnection *watchConnection):
    QObject(pebble),
    m_pebble(pebble),
    m_watchConnection(watchConnection)
{
    m_watchConnection->registerEndpointHandler(WatchConnection::EndpointBlobDB, this, "notificationReply");
}

void NotificationEndpoint::sendLegacyNotification(Pebble::NotificationType type, const QString &sender, const QString &data, const QString &subject)
{
    LegacyNotification::Source source = LegacyNotification::SourceSMS;
    switch (type) {
    case Pebble::NotificationTypeEmail:
        source = LegacyNotification::SourceEmail;
        break;
    case Pebble::NotificationTypeFacebook:
        source = LegacyNotification::SourceFacebook;
        break;
    case Pebble::NotificationTypeSMS:
        source = LegacyNotification::SourceSMS;
        break;
    case Pebble::NotificationTypeTwitter:
        source = LegacyNotification::SourceTwitter;
        break;
    }

    LegacyNotification notification(source, sender, data, QDateTime::currentDateTime(), subject);
    m_watchConnection->writeToPebble(WatchConnection::EndpointNotification, notification.serialize());
}

void NotificationEndpoint::notificationReply(const QByteArray &data)
{
    qDebug() << "have notification reply" << data.toHex();

}
