#ifndef UBUNTUPLATFORM_H
#define UBUNTUPLATFORM_H

#include "libpebble/platforminterface.h"

#include <QDBusInterface>
#include <TelepathyQt/AbstractClientObserver>

class QDBusPendingCallWatcher;
class TelepathyMonitor;

class UbuntuPlatform : public PlatformInterface
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
    Q_PROPERTY(QDBusInterface* interface READ interface)


public:
    UbuntuPlatform(QObject *parent = 0);
    QDBusInterface* interface() const;

    void sendMusicControlComand(Pebble::MusicControl controlButton) override;
    MusicMetaData musicMetaData() const override;

    void hangupCall(uint cookie) override;

public slots:
    QDBusPendingReply<uint> Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expire_timeout);
    void CloseNotification(uint id);


private slots:
    void fetchMusicMetadata();
    void fetchMusicMetadataFinished(QDBusPendingCallWatcher *watcher);

    void onIncomingCall(uint cookie, const QString &number, const QString &name);
    void onCallStarted(uint cookie);
    void onCallEnded(uint cookie, bool missed);

private:
    QDBusInterface *m_iface;

    QString m_mprisService;
    MusicMetaData m_musicMetaData;

    TelepathyMonitor *m_telepathyMonitor;
};

#endif // UBUNTUPLATFORM_H
