#include "notificationmanager.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

NotificationManager::NotificationManager(QObject *parent) : QObject(parent)
{

}

void NotificationManager::injectNotification(const QString &source, Pebble::NotificationType type, const QString &from, const QString &title, const QString &message)
{
    QString settingsFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/upebble.mzanetti/notifications.conf";
    qDebug() << "Settings file" << settingsFile;
    QSettings settings(settingsFile, QSettings::IniFormat);
    if (!settings.contains(source)) {
        settings.setValue(source, true);
    }
    qDebug() << "NotificationManager: incoming notification" << from << title << message;
    if (settings.value(source).toBool()) {
        emit displayNotification(type, from, title, message);
    }
}

