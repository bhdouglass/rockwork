#include "notificationendpoint.h"

#include "watchconnection.h"
#include "pebble.h"

#include <QDebug>
#include <QDateTime>

NotificationEndpoint::NotificationEndpoint(Pebble *pebble, WatchConnection *watchConnection):
    QObject(pebble),
    m_pebble(pebble),
    m_watchConnection(watchConnection)
{
    m_watchConnection->registerEndpointHandler(WatchConnection::EndpointBlobDB, this, "notificationReply");
}

void NotificationEndpoint::sendNotification(Pebble::NotificationType type, const QString &sender, const QString &subject, const QString &data)
{
    switch (m_pebble->hardwarePlatform()) {
    case Pebble::HardwarePlatformUnknown:
        qDebug() << "Unknown Hardware platform. Cannot send notification.";
        break;
    case Pebble::HardwarePlatformAplite: {
        QStringList tmp;
        tmp.append(sender);
        tmp.append(data);
        tmp.append(QString::number(QDateTime::currentMSecsSinceEpoch()));
        tmp.append(subject);

        QByteArray res = m_watchConnection->buildMessageData(0, tmp);

        m_watchConnection->writeToPebble(WatchConnection::EndpointNotification, res);
        }
        break;
    case Pebble::HardwarePlatformBasalt:
    case Pebble::HardwarePlatformChalk: {
        int source;
        switch (type) {
        case Pebble::NotificationTypeEmail:
            source = 19;
            break;
//        case leadFACEBOOK:
//            source = 11;
//            break;
        case Pebble::NotificationTypeSMS:
            source = 45;
            break;
//        case leadTWITTER:
//            source = 6;
//            break;
        default:
            source = 1;
        }

        int attributesCount = 0;
        QByteArray attributes;

        attributesCount++;
        QByteArray senderBytes = sender.left(64).toUtf8();
        attributes.append(0x01); // id = title
        attributes.append(senderBytes.length() & 0xFF); attributes.append(((senderBytes.length() >> 8) & 0xFF)); // length
        attributes.append(senderBytes); // content

        attributesCount++;
        attributes.append(0x04); // id = tinyicon
        attributes.append(0x04); attributes.append('\0'); // length
        attributes.append(source); attributes.append('\0'); attributes.append('\0'); attributes.append('\0'); // content

        attributesCount++;
        QByteArray subjectBytes = (subject.isEmpty() ? data : subject).left(64).toUtf8();
        attributes.append(0x02); // id = subtitle
        attributes.append(subjectBytes.length() & 0xFF); attributes.append((subjectBytes.length() >> 8) & 0xFF); // length
        attributes.append(subjectBytes); //content

        if (!data.isEmpty()) {
            attributesCount++;
            QByteArray dataBytes = data.left(512).toUtf8();
            attributes.append(0x03); // id = body
            attributes.append(dataBytes.length() & 0xFF); attributes.append((dataBytes.length() >> 8) & 0xFF); // length
            attributes.append(dataBytes); // content
        }

        qDebug() << "have attributes" << source << attributes.toHex();

        QByteArray actions;
        actions.append('\0'); // action id
        actions.append(0x04); // type = dismiss
        actions.append(0x01); // attributes length = 1
        actions.append(0x01); // attribute id = title
        actions.append(0x07); actions.append('\0'); // attribute length
        actions.append("Dismiss"); // attribute content


        QByteArray itemId = QUuid::createUuid().toRfc4122();
        int time = QDateTime::currentMSecsSinceEpoch() / 1000;
        QByteArray item;
        item.append(itemId); // item id
        item.append(QUuid().toRfc4122()); // parent id
        item.append(time & 0xFF); item.append((time >> 8) & 0xFF); item.append((time >> 16) & 0xFF); item.append((time >> 24) & 0xFF); // timestamp
        item.append('\0'); item.append('\0'); // duration
        item.append(0x01); // type: notification
        item.append('\0'); item.append('\0'); // flags
        item.append(0x01); // layout

        int length = attributes.length() + actions.length();
        item.append(length & 0xFF); item.append((length >> 8) & 0xFF); // data length
        item.append(attributesCount); // attributes count
        item.append(0x01); // actions count
        item.append(attributes);
        item.append(actions);

        int token = (qrand() % ((int)pow(2, 16) - 2)) + 1;
        QByteArray blob;
        blob.append(0x01); // command = insert
        blob.append(token & 0xFF); blob.append((token >> 8) & 0xFF); // token
        blob.append(0x04); //database id = notification
        blob.append(itemId.length() & 0xFF); // key length
        blob.append(itemId); // key
        blob.append(item.length() & 0xFF); blob.append((item.length() >> 8) & 0xFF); // value length
        blob.append(item);

        m_watchConnection->writeToPebble(WatchConnection::EndpointBlobDB, blob);
        }
        break;
    default:
//        qCWarning(l) << "Tried sending notification to unsupported watch platform" << platform << ":" << lead << sender << data << subject;
        break;
    }

}

void NotificationEndpoint::notificationReply(const QByteArray &data)
{
    qDebug() << "have notification reply" << data.toHex();

}
