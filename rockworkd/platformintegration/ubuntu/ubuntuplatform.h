#ifndef UBUNTUPLATFORM_H
#define UBUNTUPLATFORM_H

#include "libpebble/platforminterface.h"
#include "libpebble/enums.h"

#include <QDBusInterface>
#include <TelepathyQt/AbstractClientObserver>

#include <qdbusactiongroup.h>
#include <qstateaction.h>

class QDBusPendingCallWatcher;
class TelepathyMonitor;
class OrganizerAdapter;
class SyncMonitorClient;

class UbuntuPlatform : public PlatformInterface, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
    Q_PROPERTY(QDBusInterface* interface READ interface)


public:
    UbuntuPlatform(QObject *parent = 0);
    QDBusInterface* interface() const;

    void sendMusicControlCommand(MusicControlCommand controlCommand) override;
    MusicMetaData musicMetaData() const override;
    MusicPlayState musicPlayState() const override;

    void hangupCall(uint cookie) override;

    QList<CalendarEvent> organizerItems() const override;

    void actionTriggered(const QString &actToken) override;

public slots:
    uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expire_timeout);


private slots:
    void setupMusicService();
    void mprisPropertiesChanged(const QString &interface, const QVariantMap &changedProps, const QStringList &invalidatedProps);

    void fetchPropertyAsync(const QString &propertyName);
    void propertyChanged(const QString &propertyName, const QVariant &value);

private:
    QDBusInterface *m_iface;

    QString m_mprisService;
    MusicMetaData m_musicMetaData;
    MusicPlayState m_musicPlayState;
    QDBusActionGroup m_volumeActionGroup;
    QStateAction *m_volumeAction = nullptr;

    TelepathyMonitor *m_telepathyMonitor;
    OrganizerAdapter *m_organizerAdapter;
    SyncMonitorClient *m_syncMonitorClient;
    QTimer m_syncTimer;
};

#endif // UBUNTUPLATFORM_H
