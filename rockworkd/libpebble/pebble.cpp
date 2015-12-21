#include "pebble.h"
#include "watchconnection.h"
#include "notificationendpoint.h"
#include "watchdatareader.h"
#include "musicendpoint.h"
#include "phonecallendpoint.h"
#include "appmanager.h"
#include "appmsgmanager.h"
#include "jskitmanager.h"
#include "blobdb.h"
#include "appdownloader.h"
#include "core.h"
#include "platforminterface.h"

#include <QDateTime>
#include <QStandardPaths>

Pebble::Pebble(QObject *parent) : QObject(parent)
{
    m_storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    m_connection = new WatchConnection(this);
    QObject::connect(m_connection, &WatchConnection::watchConnected, this, &Pebble::onPebbleConnected);
    QObject::connect(m_connection, &WatchConnection::watchDisconnected, this, &Pebble::onPebbleDisconnected);

    m_connection->registerEndpointHandler(WatchConnection::EndpointVersion, this, "pebbleVersionReceived");
    m_connection->registerEndpointHandler(WatchConnection::EndpointPhoneVersion, this, "phoneVersionAsked");
    m_connection->registerEndpointHandler(WatchConnection::EndpointDataLogging, this, "logData");
    m_notificationEndpoint = new NotificationEndpoint(this, m_connection);
    m_musicEndpoint = new MusicEndpoint(this, m_connection);
    m_phoneCallEndpoint = new PhoneCallEndpoint(this, m_connection);
    QObject::connect(m_phoneCallEndpoint, &PhoneCallEndpoint::hangupCall, this, &Pebble::hangupCall);
    m_appManager = new AppManager(this, m_connection);
    QObject::connect(m_appManager, &AppManager::appsChanged, this, &Pebble::installedAppsChanged);
    m_appMsgManager = new AppMsgManager(m_appManager, m_connection, this);
    m_jskitManager = new JSKitManager(this, m_connection, m_appManager, m_appMsgManager, this);
    m_blobDB = new BlobDB(this, m_connection);
    QObject::connect(m_blobDB, &BlobDB::muteSource, this, &Pebble::muteNotificationSource);
    QObject::connect(m_blobDB, &BlobDB::actionTriggered, this, &Pebble::actionTriggered);
    m_appDownloader = new AppDownloader(m_storagePath, this);
    QObject::connect(m_appDownloader, &AppDownloader::downloadFinished, this, &Pebble::appDownloadFinished);

    m_appManager->rescan();
}

QBluetoothAddress Pebble::address() const
{
    return m_address;
}

void Pebble::setAddress(const QBluetoothAddress &address)
{
    m_address = address;
}

QString Pebble::name() const
{
    return m_name;
}

void Pebble::setName(const QString &name)
{
    m_name = name;
}

QBluetoothLocalDevice::Pairing Pebble::pairingStatus() const
{
    QBluetoothLocalDevice dev;
    return dev.pairingStatus(m_address);
}

bool Pebble::connected() const
{
    return m_connection->isConnected() && !m_serialNumber.isEmpty();
}

void Pebble::connect()
{
    qDebug() << "Connecting to Pebble:" << m_name << m_address;
    m_connection->connectPebble(m_address);
}

QDateTime Pebble::softwareBuildTime() const
{
    return m_softwareBuildTime;
}

QString Pebble::softwareVersion() const
{
    return m_softwareVersion;
}

QString Pebble::softwareCommitRevision() const
{
    return m_softwareCommitRevision;
}

HardwareRevision Pebble::hardwareRevision() const
{
    return m_hardwareRevision;
}

void Pebble::setHardwareRevision(HardwareRevision hardwareRevision)
{
    m_hardwareRevision = hardwareRevision;
    switch (m_hardwareRevision) {
    case HardwareRevisionUNKNOWN:
        m_hardwarePlatform = HardwarePlatformUnknown;
        break;
    case HardwareRevisionTINTIN_EV1:
    case HardwareRevisionTINTIN_EV2:
    case HardwareRevisionTINTIN_EV2_3:
    case HardwareRevisionTINTIN_EV2_4:
    case HardwareRevisionTINTIN_V1_5:
    case HardwareRevisionBIANCA:
    case HardwareRevisionTINTIN_BB:
    case HardwareRevisionTINTIN_BB2:
        m_hardwarePlatform = HardwarePlatformAplite;
        break;
    case HardwareRevisionSNOWY_EVT2:
    case HardwareRevisionSNOWY_DVT:
    case HardwareRevisionBOBBY_SMILES:
    case HardwareRevisionSNOWY_BB:
    case HardwareRevisionSNOWY_BB2:
        m_hardwarePlatform = HardwarePlatformBasalt;
        break;
    case HardwareRevisionSPALDING_EVT:
    case HardwareRevisionSPALDING:
    case HardwareRevisionSPALDING_BB2:
        m_hardwarePlatform = HardwarePlatformChalk;
        break;
    }
}

HardwarePlatform Pebble::hardwarePlatform() const
{
    return m_hardwarePlatform;
}

QString Pebble::serialNumber() const
{
    return m_serialNumber;
}

bool Pebble::isUnfaithful() const
{
    return m_isUnfaithful;
}

void Pebble::sendNotification(const Notification &notification)
{
    qDebug() << "should send notification from:" << notification.sender() << "subject" << notification.subject() << "data" << notification.body();
    if (m_softwareVersion < "v3.0") {
        m_notificationEndpoint->sendLegacyNotification(notification);
    } else {
        m_blobDB->insertNotification(notification);
    }
}

void Pebble::setMusicMetadata(const MusicMetaData &metaData)
{
    m_musicEndpoint->setMusicMetadata(metaData);
}

void Pebble::insertReminder()
{
    m_blobDB->insertReminder();
}

void Pebble::clearAppDB()
{
    m_blobDB->clearApps();
}

void Pebble::incomingCall(uint cookie, const QString &number, const QString &name)
{
    m_phoneCallEndpoint->incomingCall(cookie, number, name);
}

void Pebble::callStarted(uint cookie)
{
    m_phoneCallEndpoint->callStarted(cookie);
}

void Pebble::callEnded(uint cookie, bool missed)
{
    m_phoneCallEndpoint->callEnded(cookie, missed);
}

void Pebble::clearTimeline()
{
    m_blobDB->clearTimeline();
}

void Pebble::syncCalendar(const QList<CalendarEvent> items)
{
    if (connected()) {
        m_blobDB->syncCalendar(items);
    }
}

void Pebble::installApp(const QString &id)
{
    m_appDownloader->downloadApp(id);
}

QList<QString> Pebble::installedAppIds()
{
    return m_appManager->appIds();
}

AppInfo Pebble::appInfo(const QString &id)
{
    return m_appManager->info(id);
}

void Pebble::removeApp(const QString &id)
{
    m_blobDB->removeApp(m_appManager->info(id));
    m_appManager->removeApp(id);
}

void Pebble::onPebbleConnected()
{
    qDebug() << "Pebble connected:" << m_name;
    m_connection->writeToPebble(WatchConnection::EndpointVersion, QByteArray(1, 0));
    emit pebbleConnected();
}

void Pebble::onPebbleDisconnected()
{
    qDebug() << "Pebble disconnected:" << m_name;
    emit pebbleDisconnected();
}

void Pebble::pebbleVersionReceived(const QByteArray &data)
{
    WatchDataReader wd(data);

    qDebug() << "blubb" << data.toHex();

    wd.skip(1);
    m_softwareBuildTime = QDateTime::fromTime_t(wd.read<quint32>());
    qDebug() << "Software Version build:" << m_softwareBuildTime;
    m_softwareVersion = wd.readFixedString(32);
    qDebug() << "Software Version string:" << m_softwareVersion;
    m_softwareCommitRevision = wd.readFixedString(8);
    qDebug() << "Software Version commit:" << m_softwareCommitRevision;

    qDebug() << "Recovery:" << wd.read<quint8>();
    HardwareRevision rev = (HardwareRevision)wd.read<quint8>();
    setHardwareRevision(rev);
    qDebug() << "HW Revision:" << rev;
    qDebug() << "Metadata Version:" << wd.read<quint8>();

    qDebug() << "Safe build:" << QDateTime::fromTime_t(wd.read<quint32>());
    qDebug() << "Safe version:" << wd.readFixedString(32);
    qDebug() << "safe commit:" << wd.readFixedString(8);
    qDebug() << "Safe recovery:" << wd.read<quint8>();
    qDebug() << "HW Revision:" << wd.read<quint8>();
    qDebug() << "Metadata Version:" << wd.read<quint8>();

    qDebug() << "BootloaderBuild" << QDateTime::fromTime_t(wd.read<quint32>());
    qDebug() << "hardwareRevision" << wd.readFixedString(9);
    m_serialNumber = wd.readFixedString(12);
    qDebug() << "serialnumber" << m_serialNumber;
    qDebug() << "BT address" << wd.readBytes(6);
    qDebug() << "CRC:" << wd.read<quint32>();
    qDebug() << "Resource timestamp:" << QDateTime::fromTime_t(wd.read<quint32>());
    qDebug() << "Language" << wd.readFixedString(6);
    qDebug() << "Language version" << wd.read<quint16>();
    qDebug() << "Capabilities" << wd.read<quint64>();
    m_isUnfaithful = wd.read<quint8>();
    qDebug() << "Is Unfaithful" << m_isUnfaithful;

    // This is useful for debugging
//    m_isUnfaithful = true;

    if (m_isUnfaithful) {
        qDebug() << "Pebble has been unfaithful. Clearing it up.";
        clearTimeline();
        clearAppDB();
        foreach (const QUuid &appUuid, m_appManager->appUuids()) {
            m_blobDB->insertAppMetaData(m_appManager->info(appUuid));
        }
    }

    syncCalendar(Core::instance()->platform()->organizerItems());

    emit pebbleConnected();
}

void Pebble::phoneVersionAsked(const QByteArray &data)
{
    qDebug() << "sending phone version" << data.toHex();
    unsigned int sessionCap = 0x80000000;
    unsigned int remoteCap = 16 | 32 | WatchConnection::OSAndroid;

    QByteArray res;

    //Prefix
    res.append(0x01);
    res.append(0xff);
    res.append(0xff);
    res.append(0xff);
    res.append(0xff);

    //Session Capabilities
    res.append((char)((sessionCap >> 24) & 0xff));
    res.append((char)((sessionCap >> 16) & 0xff));
    res.append((char)((sessionCap >> 8) & 0xff));
    res.append((char)(sessionCap & 0xff));

    //Remote Capabilities
    res.append((char)((remoteCap >> 24) & 0xff));
    res.append((char)((remoteCap >> 16) & 0xff));
    res.append((char)((remoteCap >> 8) & 0xff));
    res.append((char)(remoteCap & 0xff));

    //Version Magic
    res.append((char)0x02);

    //Append Version
    res.append((char)0x02); //Major
    res.append((char)0x00); //Minor
    res.append((char)0x00); //Bugfix

    m_connection->writeToPebble(WatchConnection::EndpointPhoneVersion, res);
}

void Pebble::logData(const QByteArray &/*data*/)
{
    //    qDebug() << "Data logged:" << data.toHex();
}

void Pebble::appDownloadFinished(const QString &id)
{
    m_appManager->scanApp(m_storagePath + "/apps/" + id);
    m_blobDB->insertAppMetaData(m_appManager->info(id));

}
