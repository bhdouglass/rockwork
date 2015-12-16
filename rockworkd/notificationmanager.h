#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include "libpebble/pebble.h"

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = 0);

    void injectNotification(const Notification &notification);

    void muteSource(const QString &source);

signals:
    void displayNotification(const Notification &notification);

};

#endif // NOTIFICATIONMANAGER_H
