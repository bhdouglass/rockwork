#ifndef PEBBLE_H
#define PEBBLE_H

#include "musicmetadata.h"

#include <QObject>
#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QTimer>

class WatchConnection;
class NotificationEndpoint;
class MusicEndpoint;
class PhoneCallEndpoint;
class AppsEndpoint;

class Pebble : public QObject
{
    Q_OBJECT
    Q_ENUMS(Pebble::NotificationType)
    Q_PROPERTY(QBluetoothAddress address MEMBER m_address)
    Q_PROPERTY(HardwareRevision HardwareRevision READ hardwareRevision)
    Q_PROPERTY(HardwarePlatform hardwarePlatform MEMBER m_hardwarePlatform)

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

    enum NotificationType {
        NotificationTypeEmail,
        NotificationTypeSMS
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

    HardwareRevision hardwareRevision() const;
    HardwarePlatform hardwarePlatform() const;

public slots:
    void sendNotification(NotificationType type, const QString &sender, const QString &subject, const QString &data);
    void setMusicMetadata(const MusicMetaData &metaData);

    void incomingCall(uint cookie, const QString &number, const QString &name);
    void callStarted(uint cookie);
    void callEnded(uint cookie, bool missed);

private slots:
    void onPebbleConnected();
    void onPebbleDisconnected();
    void pebbleVersionReceived(const QByteArray &data);
    void phoneVersionAsked(const QByteArray &data);
    void logData(const QByteArray &data);

signals:
    void pebbleConnected();
    void pebbleDisconnected();
    void musicControlPressed(MusicControl control);
    void hangupCall(uint cookie);

private:
    void setHardwareRevision(HardwareRevision hardwareRevision);


    QBluetoothAddress m_address;
    QString m_name;
    HardwareRevision m_hardwareRevision;
    HardwarePlatform m_hardwarePlatform;

    WatchConnection *m_connection;
    NotificationEndpoint *m_notificationEndpoint;
    MusicEndpoint *m_musicEndpoint;
    PhoneCallEndpoint *m_phoneCallEndpoint;
    AppsEndpoint *m_appsEndpoint;
};

#endif // PEBBLE_H
