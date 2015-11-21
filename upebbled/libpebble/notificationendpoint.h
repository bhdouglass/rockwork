#include <QObject>

#include "pebble.h"

class WatchConnection;

class NotificationEndpoint: public QObject
{
    Q_OBJECT
public:
    NotificationEndpoint(Pebble *pebble, WatchConnection *watchConnection);

    void sendNotification(Pebble::NotificationType type, const QString &sender, const QString &data, const QString &subject);

private slots:
    void notificationReply(const QByteArray &data);

private:
    Pebble *m_pebble;
    WatchConnection *m_watchConnection;
};
