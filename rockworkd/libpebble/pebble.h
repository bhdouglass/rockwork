#ifndef PEBBLE_H
#define PEBBLE_H

#include "musicmetadata.h"
#include "notification.h"
#include "calendarevent.h"

#include <QObject>
#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QTimer>

class WatchConnection;
class NotificationEndpoint;
class MusicEndpoint;
class PhoneCallEndpoint;
class AppManager;
class AppMsgManager;
class BankManager;
class JSKitManager;
class BlobDB;
class AppDownloader;

class Pebble : public QObject
{
    Q_OBJECT
    Q_ENUMS(Pebble::NotificationType)
    Q_PROPERTY(QBluetoothAddress address MEMBER m_address)
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(HardwareRevision HardwareRevision READ hardwareRevision)
    Q_PROPERTY(HardwarePlatform hardwarePlatform MEMBER m_hardwarePlatform)
    Q_PROPERTY(QString softwareVersion MEMBER m_softwareVersion)
    Q_PROPERTY(QString serialNumber MEMBER m_serialNumber)

public:
    enum HardwareRevision {
        HardwareRevisionUNKNOWN = 0,
        HardwareRevisionTINTIN_EV1 = 1,
        HardwareRevisionTINTIN_EV2 = 2,
        HardwareRevisionTINTIN_EV2_3 = 3,
        HardwareRevisionTINTIN_EV2_4 = 4,
        HardwareRevisionTINTIN_V1_5 = 5,
        HardwareRevisionBIANCA = 6,
        HardwareRevisionSNOWY_EVT2 = 7,
        HardwareRevisionSNOWY_DVT = 8,
        HardwareRevisionSPALDING_EVT = 9,
        HardwareRevisionBOBBY_SMILES = 10,
        HardwareRevisionSPALDING = 11,

        HardwareRevisionTINTIN_BB = 0xFF,
        HardwareRevisionTINTIN_BB2 = 0xFE,
        HardwareRevisionSNOWY_BB = 0xFD,
        HardwareRevisionSNOWY_BB2 = 0xFC,
        HardwareRevisionSPALDING_BB2 = 0xFB
    };

    enum HardwarePlatform {
        HardwarePlatformUnknown = 0,
        HardwarePlatformAplite,
        HardwarePlatformBasalt,
        HardwarePlatformChalk
    };

    enum MusicControl {
        MusicControlPlayPause,
        MusicControlSkipBack,
        MusicControlSkipNext,
        MusicControlVolumeUp,
        MusicControlVolumeDown
    };
    enum CallStatus {
        CallStatusIncoming,
        CallStatusOutGoing
    };

    explicit Pebble(QObject *parent = 0);

    QBluetoothAddress address() const;
    void setAddress(const QBluetoothAddress &address);

    QString name() const;
    void setName(const QString &name);

    QBluetoothLocalDevice::Pairing pairingStatus() const;

    bool connected() const;
    void connect();

    QDateTime softwareBuildTime() const;
    QString softwareVersion() const;
    QString softwareCommitRevision() const;
    HardwareRevision hardwareRevision() const;
    HardwarePlatform hardwarePlatform() const;
    QString serialNumber() const;
    bool isUnfaithful() const;

public slots:
    void sendNotification(const Notification &notification);
    void setMusicMetadata(const MusicMetaData &metaData);

    void incomingCall(uint cookie, const QString &number, const QString &name);
    void callStarted(uint cookie);
    void callEnded(uint cookie, bool missed);

    void clearTimeline();
    void syncCalendar(const QList<CalendarEvent> items);

    void insertReminder();

    void clearAppDB();
    void installApp(const QString &id);

private slots:
    void onPebbleConnected();
    void onPebbleDisconnected();
    void pebbleVersionReceived(const QByteArray &data);
    void phoneVersionAsked(const QByteArray &data);
    void logData(const QByteArray &data);
    void appDownloadFinished(const QString &id);

signals:
    void pebbleConnected();
    void pebbleDisconnected();
    void musicControlPressed(MusicControl control);
    void hangupCall(uint cookie);
    void muteNotificationSource(const QString &source);
    void actionTriggered(const QString &actToken);

private:
    void setHardwareRevision(HardwareRevision hardwareRevision);

    QBluetoothAddress m_address;
    QString m_name;
    QDateTime m_softwareBuildTime;
    QString m_softwareVersion;
    QString m_softwareCommitRevision;
    HardwareRevision m_hardwareRevision;
    HardwarePlatform m_hardwarePlatform;
    QString m_serialNumber;
    bool m_isUnfaithful = false;

    WatchConnection *m_connection;
    NotificationEndpoint *m_notificationEndpoint;
    MusicEndpoint *m_musicEndpoint;
    PhoneCallEndpoint *m_phoneCallEndpoint;
    AppManager *m_appManager;
    AppMsgManager *m_appMsgManager;
    JSKitManager *m_jskitManager;
    BankManager *m_bankManager;
    BlobDB *m_blobDB;
    AppDownloader *m_appDownloader;

    QString m_storagePath;
};

#endif // PEBBLE_H
