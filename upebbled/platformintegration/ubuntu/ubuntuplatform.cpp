#include "ubuntuplatform.h"

#include "callchannelobserver.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>

UbuntuPlatform::UbuntuPlatform(QObject *parent):
    PlatformInterface(parent)
{
    // Notifications
    QDBusConnection::sessionBus().registerObject("/org/freedesktop/Notifications", this, QDBusConnection::ExportAllSlots);
    m_iface = new QDBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
    m_iface->call("AddMatch", "interface='org.freedesktop.Notifications',member='Notify',type='method_call',eavesdrop='true'");
    m_iface->call("AddMatch", "interface='org.freedesktop.Notifications',member='CloseNotification',type='method_call',eavesdrop='true'");

    // Music
    QDBusConnectionInterface *iface = QDBusConnection::sessionBus().interface();
    const QStringList &services = iface->registeredServiceNames();
    foreach (QString service, services) {
        if (service.startsWith("org.mpris.MediaPlayer2.")) {
            qDebug() << "have mpris service" << service;
            m_mprisService = service;
            fetchMusicMetadata();
            break;
        }
    }

    // Calls
    m_telepathyMonitor = new TelepathyMonitor(this);
    connect(m_telepathyMonitor, &TelepathyMonitor::incomingCall, this, &UbuntuPlatform::onIncomingCall);
    connect(m_telepathyMonitor, &TelepathyMonitor::callStarted, this, &UbuntuPlatform::onCallStarted);
    connect(m_telepathyMonitor, &TelepathyMonitor::callEnded, this, &UbuntuPlatform::onCallEnded);
}

QDBusInterface *UbuntuPlatform::interface() const
{
    return m_iface;
}

QDBusPendingReply<uint> UbuntuPlatform::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expire_timeout)
{
    QStringList hiddenNotifications = {"indicator-sound", "indicator-network"};
    if (!hiddenNotifications.contains(app_name)) {
        if (hints.contains("x-canonical-secondary-icon") && hints.value("x-canonical-secondary-icon").toString() == "incoming-call") {
            qDebug() << "have phone call notification";
//            emit incomingCall(m_callCookie++, body, summary);
        } else {
            qDebug() << "Notification received" << app_name << replaces_id << app_icon << summary << body << actions << hints << expire_timeout;
            emit notificationReceived(app_name, Pebble::NotificationTypeSMS, summary, QString(), body);
        }
    }
    // Make sure the real notification server returns something before we do or we mess up the system
    QDateTime end = QDateTime::currentDateTime().addMSecs(500);
    while (QDateTime::currentDateTime() < end) {
        qApp->processEvents();
    }
    return QDBusPendingReply<uint>();
}

void UbuntuPlatform::CloseNotification(uint id)
{
    qDebug() << "should close notification" << id;
//    emit callEnded(m_callCookie, false);
//    emit callEnded(m_callCookie, true);
}

void UbuntuPlatform::sendMusicControlComand(Pebble::MusicControl controlButton)
{
    QString method;
    switch (controlButton) {
    case Pebble::MusicControlPlayPause:
        method = "PlayPause";
        break;
    case Pebble::MusicControlSkipBack:
        method = "Previous";
        break;
    case Pebble::MusicControlSkipNext:
        method = "Next";
        break;
    }

    if (!method.isEmpty()) {
        QDBusMessage call = QDBusMessage::createMethodCall(m_mprisService, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", method);
        QDBusError err = QDBusConnection::sessionBus().call(call);

        if (err.isValid()) {
            qWarning() << "while calling mpris method on" << m_mprisService << ":" << err.message();
        }
        return;
    }
}

MusicMetaData UbuntuPlatform::musicMetaData() const
{
    return m_musicMetaData;
}

void UbuntuPlatform::hangupCall(uint cookie)
{
    m_telepathyMonitor->hangupCall(cookie);
}

void UbuntuPlatform::fetchMusicMetadata()
{
    if (!m_mprisService.isEmpty()) {
        QDBusMessage call = QDBusMessage::createMethodCall(m_mprisService, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Get");
        call << "org.mpris.MediaPlayer2.Player" << "Metadata";
        QDBusPendingCall pcall = QDBusConnection::sessionBus().asyncCall(call);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &UbuntuPlatform::fetchMusicMetadataFinished);
    }
}

void UbuntuPlatform::fetchMusicMetadataFinished(QDBusPendingCallWatcher *watcher)
{
    watcher->deleteLater();
    QDBusReply<QDBusVariant> reply = watcher->reply();
    if (reply.isValid()) {
        QVariantMap curMetadata = qdbus_cast<QVariantMap>(reply.value().variant().value<QDBusArgument>());
        m_musicMetaData.artist = curMetadata.value("xesam:artist").toString();
        m_musicMetaData.album = curMetadata.value("xesam:album").toString();
        m_musicMetaData.title = curMetadata.value("xesam:title").toString();
        emit musicMetadataChanged(m_musicMetaData);
    } else {
        qWarning() << reply.error().message();
    }
}

void UbuntuPlatform::onIncomingCall(uint cookie, const QString &number, const QString &name)
{
    emit incomingCall(cookie, number, name);
}

void UbuntuPlatform::onCallStarted(uint cookie)
{
    emit callStarted(cookie);
}

void UbuntuPlatform::onCallEnded(uint cookie, bool missed)
{
    emit callEnded(cookie, missed);
}

