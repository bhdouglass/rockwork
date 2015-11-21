#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include "libpebble/pebble.h"

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = 0);

    void injectNotification(const QString &source, Pebble::NotificationType type, const QString &from, const QString &title, const QString &message);

signals:
    void displayNotification(Pebble::NotificationType type, const QString &from, const QString &title, const QString &message);

};

#endif // NOTIFICATIONMANAGER_H
