#include "notificationmanager.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

NotificationManager::NotificationManager(QObject *parent) : QObject(parent)
{

}

void NotificationManager::injectNotification(const Notification &notification)
{
    QString settingsFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/rockwork.mzanetti/notifications.conf";
    QSettings settings(settingsFile, QSettings::IniFormat);
    if (!settings.contains(notification.sourceId())) {
        settings.setValue(notification.sourceId(), true);
    }
    qDebug() << "NotificationManager: incoming notification" << notification.sender() << notification.subject() << notification.body();
    if (settings.value(notification.sourceId()).toBool()) {
        emit displayNotification(notification);
    }
}

void NotificationManager::muteSource(const QString &source)
{
    QString settingsFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/rockwork.mzanetti/notifications.conf";
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.setValue(source, false);
}

