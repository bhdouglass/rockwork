#ifndef WATCHCONNECTION_H
#define WATCHCONNECTION_H

#include <QObject>
#include <QBluetoothAddress>
#include <QBluetoothSocket>
#include <QBluetoothLocalDevice>
#include <QtEndian>
#include <QPointer>
#include <QTimer>
#include <QFile>

class EndpointHandlerInterface;
class UploadManager;

class PebblePacket {
public:
    PebblePacket() {}
    virtual ~PebblePacket() = default;
    virtual QByteArray serialize() const = 0;
    QByteArray packString(const QString &string) const {
        QByteArray tmp = string.left(0xEF).toUtf8();
        QByteArray ret;
        ret.append((tmp.length() + 1) & 0xFF);
        ret.append(tmp);
        ret.append('\0');
        return ret;
    }
};

class Callback
{
public:
    QPointer<QObject> obj;
    QString method;
};

class WatchConnection : public QObject
{
    Q_OBJECT
public:

    enum Endpoint {
        EndpointZero = 0, // We get that sometimes... nut sure yet what it is...
        EndpointTime = 11,
        EndpointVersion = 16,
        EndpointPhoneVersion = 17,
//        watchSYSTEM_MESSAGE = 18,
        EndpointMusicControl = 32,
        EndpointPhoneControl = 33,
        EndpointApplicationMessage = 48,
        EndpointLauncher = 49,
//        watchLOGS = 2000,
//        watchPING = 2001,
//        watchLOG_DUMP = 2002,
//        watchRESET = 2003,
//        watchAPP = 2004,
//        watchAPP_LOGS = 2006,
        EndpointNotification = 3000,
//        watchEXTENSIBLE_NOTIFS = 3010, // Deprecated in 3.x
//        watchRESOURCE = 4000,
//        watchFACTORY_SETTINGS = 5001,
        EndpointAppManager = 6000, // Deprecated in 3.x
        EndpointAppFetch = 6001, // New in 3.x
        EndpointDataLogging = 6778,
//        watchSCREENSHOT = 8000,
//        watchFILE_MANAGER = 8181,
//        watchCORE_DUMP = 9000,
//        watchAUDIO = 10000, // New in 3.x
        EndpointActionHandler = 11440,
        EndpointBlobDB = 45531, // New in 3.x
        EndpointPutBytes = 48879
    };

    enum OS {
         OSUnknown = 0,
         OSiOS = 1,
         OSAndroid = 2,
         OSOSX = 3,
         OSLinux = 4,
         OSWindows = 5
    };

    typedef QMap<int, QVariant> Dict;
    enum DictItemType {
        DictItemTypeBytes,
        DictItemTypeString,
        DictItemTypeUInt,
        DictItemTypeInt
    };

    enum UploadType {
        UploadTypeFirmware = 1,
        UploadTypeRecovery = 2,
        UploadTypeSystemResources = 3,
        UploadTypeResources = 4,
        UploadTypeBinary = 5,
        UploadTypeFile = 6,
        UploadTypeWorker = 7
    };
    enum UploadStatus {
        UploadStatusProgress,
        UploadStatusFailed,
        UploadStatusSuccess
    };

    explicit WatchConnection(QObject *parent = 0);
    UploadManager *uploadManager() const;

    void connectPebble(const QBluetoothAddress &pebble);
    bool isConnected();

    QByteArray buildData(QStringList data);
    QByteArray buildMessageData(uint lead, QStringList data);

    void writeToPebble(Endpoint endpoint, const QByteArray &data);

    bool registerEndpointHandler(Endpoint endpoint, QObject *handler, const QString &method);

signals:
    void watchConnected();
    void watchDisconnected();
    void watchConnectionFailed();

private:
    void scheduleReconnect();
    void reconnect();

    void pebbleVersionReceived(const QByteArray &data);
    void handleLauncherMessage(const QByteArray &data);

private slots:
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode state);
    void pebbleConnected();
    void pebbleDisconnected();
    void socketError(QBluetoothSocket::SocketError error);
    void readyRead();
//    void logData(const QByteArray &data);


private:
    QBluetoothAddress m_pebbleAddress;
    QBluetoothLocalDevice *m_localDevice;
    QBluetoothSocket *m_socket = nullptr;
    int m_connectionAttempts = 0;
    QTimer m_reconnectTimer;

    UploadManager *m_uploadManager;
    QHash<Endpoint, Callback> m_endpointHandlers;
};

#endif // WATCHCONNECTION_H
