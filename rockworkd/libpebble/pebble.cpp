#include "pebble.h"
#include "watchconnection.h"
#include "notificationendpoint.h"
#include "watchdatareader.h"
#include "watchdatawriter.h"
#include "musicendpoint.h"
#include "phonecallendpoint.h"
#include "appmanager.h"
#include "appmsgmanager.h"
#include "jskitmanager.h"
#include "blobdb.h"
#include "appdownloader.h"
#include "screenshotendpoint.h"
#include "firmwaredownloader.h"
#include "core.h"
#include "platforminterface.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QSettings>

Pebble::Pebble(const QBluetoothAddress &address, QObject *parent):
    QObject(parent),
    m_address(address)
{
    m_storagePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + m_address.toString().replace(':', '_') + "/";

    m_connection = new WatchConnection(this);
    QObject::connect(m_connection, &WatchConnection::watchConnected, this, &Pebble::onPebbleConnected);
    QObject::connect(m_connection, &WatchConnection::watchDisconnected, this, &Pebble::onPebbleDisconnected);

    m_connection->registerEndpointHandler(WatchConnection::EndpointVersion, this, "pebbleVersionReceived");
    m_connection->registerEndpointHandler(WatchConnection::EndpointPhoneVersion, this, "phoneVersionAsked");
    m_connection->registerEndpointHandler(WatchConnection::EndpointDataLogging, this, "logData");
    m_connection->registerEndpointHandler(WatchConnection::EndpointFactorySettings, this, "factorySettingsReceived");

    m_notificationEndpoint = new NotificationEndpoint(this, m_connection);
    m_musicEndpoint = new MusicEndpoint(this, m_connection);
    m_phoneCallEndpoint = new PhoneCallEndpoint(this, m_connection);
    QObject::connect(m_phoneCallEndpoint, &PhoneCallEndpoint::hangupCall, this, &Pebble::hangupCall);

    m_appManager = new AppManager(this, m_connection);
    QObject::connect(m_appManager, &AppManager::appsChanged, this, &Pebble::installedAppsChanged);
    QObject::connect(m_appManager, &AppManager::idMismatchDetected, this, &Pebble::resetPebble);
    m_appMsgManager = new AppMsgManager(this, m_appManager, m_connection);
    m_jskitManager = new JSKitManager(this, m_connection, m_appManager, m_appMsgManager, this);
    QObject::connect(m_jskitManager, SIGNAL(openURL(const QString&, const QString&)), this, SIGNAL(openURL(const QString&, const QString&)));

    m_blobDB = new BlobDB(this, m_connection);
    QObject::connect(m_blobDB, &BlobDB::muteSource, this, &Pebble::muteNotificationSource);
    QObject::connect(m_blobDB, &BlobDB::actionTriggered, this, &Pebble::actionTriggered);

    m_appDownloader = new AppDownloader(m_storagePath, this);
    QObject::connect(m_appDownloader, &AppDownloader::downloadFinished, this, &Pebble::appDownloadFinished);

    m_screenshotEndpoint = new ScreenshotEndpoint(this, m_connection, this);
    QObject::connect(m_screenshotEndpoint, &ScreenshotEndpoint::screenshotAdded, this, &Pebble::screenshotAdded);
    QObject::connect(m_screenshotEndpoint, &ScreenshotEndpoint::screenshotRemoved, this, &Pebble::screenshotRemoved);

    m_firmwareDownloader = new FirmwareDownloader(this, m_connection);
    QObject::connect(m_firmwareDownloader, &FirmwareDownloader::updateAvailableChanged, this, &Pebble::slotUpdateAvailableChanged);
    QObject::connect(m_firmwareDownloader, &FirmwareDownloader::upgradingChanged, this, &Pebble::upgradingFirmwareChanged);

    QSettings s(m_storagePath + "/watchinfo.conf", QSettings::IniFormat);
    m_model = (Model)s.value("watchModel", (int)ModelUnknown).toInt();

}

QBluetoothAddress Pebble::address() const
{
    return m_address;
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

Model Pebble::model() const
{
    return m_model;
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

Capabilities Pebble::capabilities() const
{
    return m_capabilities;
}

bool Pebble::isUnfaithful() const
{
    return m_isUnfaithful;
}

bool Pebble::recovery() const
{
    return m_recovery;
}

bool Pebble::upgradingFirmware() const
{
    return m_firmwareDownloader->upgrading();
}

QString Pebble::storagePath() const
{
    return m_storagePath;
}

QHash<QString, bool> Pebble::notificationsFilter() const
{
    QHash<QString, bool> ret;
    QString settingsFile = m_storagePath + "/notifications.conf";
    QSettings s(settingsFile, QSettings::IniFormat);
    foreach (const QString &key, s.allKeys()) {
        ret.insert(key, s.value(key).toBool());
    }
    return ret;
}

void Pebble::setNotificationFilter(const QString &sourceId, bool enabled)
{
    QString settingsFile = m_storagePath + "/notifications.conf";
    QSettings s(settingsFile, QSettings::IniFormat);
    if (!s.contains(sourceId) || s.value(sourceId).toBool() != enabled) {
        s.setValue(sourceId, enabled);
        emit notificationFilterChanged(sourceId, enabled);
    }
}

void Pebble::sendNotification(const Notification &notification)
{
    if (!notificationsFilter().value(notification.sourceId(), true)) {
        qDebug() << "Notifications for" << notification.sourceId() << "disabled.";
        return;
    }
    // In case it wasn't there before, make sure to write it to the config now so it will appear in the config app.
    setNotificationFilter(notification.sourceId(), true);

    qDebug() << "Sending notification from:" << notification.sender() << "subject" << notification.subject() << "data" << notification.body();

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

QList<QUuid> Pebble::installedAppIds()
{
    return m_appManager->appUuids();
}

void Pebble::setAppOrder(const QList<QUuid> &newList)
{
    m_appManager->setAppOrder(newList);
}

AppInfo Pebble::appInfo(const QUuid &uuid)
{
    return m_appManager->info(uuid);
}

void Pebble::removeApp(const QUuid &uuid)
{
    qDebug() << "Should remove app:" << uuid;
    m_blobDB->removeApp(m_appManager->info(uuid));
    m_appManager->removeApp(uuid);
}

void Pebble::launchApp(const QUuid &uuid)
{
    m_appMsgManager->launchApp(uuid);
}

void Pebble::requestConfigurationURL(const QUuid &uuid) {
    if (m_jskitManager->currentApp().uuid() == uuid) {
        m_jskitManager->showConfiguration();
    }
    else {
        m_jskitManager->setConfigurationId(uuid);
        m_appMsgManager->launchApp(uuid);
    }
}

void Pebble::configurationClosed(const QUuid &uuid, const QString &result)
{
    if (m_jskitManager->currentApp().uuid() == uuid) {
        m_jskitManager->handleWebviewClosed(result);
    }
}

void Pebble::requestScreenshot()
{
    m_screenshotEndpoint->requestScreenshot();
}

QStringList Pebble::screenshots() const
{
    return m_screenshotEndpoint->screenshots();
}

void Pebble::removeScreenshot(const QString &filename)
{
    m_screenshotEndpoint->removeScreenshot(filename);
}

bool Pebble::firmwareUpdateAvailable() const
{
    return m_firmwareDownloader->updateAvailable();
}

QString Pebble::candidateFirmwareVersion() const
{
    return m_firmwareDownloader->candidateVersion();
}

QString Pebble::firmwareReleaseNotes() const
{
    return m_firmwareDownloader->releaseNotes();
}

void Pebble::upgradeFirmware() const
{
    m_firmwareDownloader->performUpgrade();
}

void Pebble::onPebbleConnected()
{
    qDebug() << "Pebble connected:" << m_name;
    QByteArray data;
    WatchDataWriter w(&data);
    w.write<quint8>(0); // Command fetch
    QString message = "mfg_color";
    w.writeLE<quint8>(message.length());
    w.writeFixedString(message.length(), message);
    m_connection->writeToPebble(WatchConnection::EndpointFactorySettings, data);

    m_connection->writeToPebble(WatchConnection::EndpointVersion, QByteArray(1, 0));
}

void Pebble::onPebbleDisconnected()
{
    qDebug() << "Pebble disconnected:" << m_name;
    emit pebbleDisconnected();
}

void Pebble::pebbleVersionReceived(const QByteArray &data)
{
    WatchDataReader wd(data);

    wd.skip(1);
    m_softwareBuildTime = QDateTime::fromTime_t(wd.read<quint32>());
    qDebug() << "Software Version build:" << m_softwareBuildTime;
    m_softwareVersion = wd.readFixedString(32);
    qDebug() << "Software Version string:" << m_softwareVersion;
    m_softwareCommitRevision = wd.readFixedString(8);
    qDebug() << "Software Version commit:" << m_softwareCommitRevision;

    m_recovery = wd.read<quint8>();
    qDebug() << "Recovery:" << m_recovery;
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
    qDebug() << "BT address" << wd.readBytes(6).toHex();
    qDebug() << "CRC:" << wd.read<quint32>();
    qDebug() << "Resource timestamp:" << QDateTime::fromTime_t(wd.read<quint32>());
    qDebug() << "Language" << wd.readFixedString(6);
    qDebug() << "Language version" << wd.read<quint16>();
    // Capabilities is 64 bits but QFlags can only do 32 bits. lets split it into 2 * 32.
    // only 8 bits are used atm anyways.
    m_capabilities = QFlag(wd.readLE<quint32>());
    qDebug() << "Capabilities" << QString::number(m_capabilities, 16);
    qDebug() << "Capabilities" << wd.readLE<quint32>();
    m_isUnfaithful = wd.read<quint8>();
    qDebug() << "Is Unfaithful" << m_isUnfaithful;

    // This is useful for debugging
//    m_isUnfaithful = true;

    if (!m_recovery) {
        m_appManager->rescan();

        QSettings version(m_storagePath + "/watchinfo.conf", QSettings::IniFormat);
        if (version.value("syncedWithVersion").toString() != QStringLiteral(VERSION)) {
            m_isUnfaithful = true;
        }

        if (m_isUnfaithful) {
            qDebug() << "Pebble sync state unclear. Resetting Pebble watch.";
            resetPebble();
        } else {
            syncCalendar(Core::instance()->platform()->organizerItems());
            syncApps();
        }
        version.setValue("syncedWithVersion", QStringLiteral(VERSION));

        syncTime();
    }

    m_firmwareDownloader->checkForNewFirmware();
    emit pebbleConnected();

}

void Pebble::factorySettingsReceived(const QByteArray &data)
{
    qDebug() << "have factory settings" << data.toHex();

    WatchDataReader reader(data);
    quint8 status = reader.read<quint8>();
    quint8 len = reader.read<quint8>();

    if (status != 0x01 && len != 0x04) {
        qWarning() << "Unexpected data reading factory settings";
        return;
    }
    m_model = (Model)reader.read<quint32>();
    QSettings s(m_storagePath + "/watchinfo.conf", QSettings::IniFormat);
    s.setValue("watchModel", m_model);
}

void Pebble::phoneVersionAsked(const QByteArray &data)
{
    qDebug() << "sending phone version" << data.toHex();
    unsigned int sessionCap = 0x80000000;
    unsigned int remoteCap = 16 | 32 | OSAndroid;

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
    QUuid uuid = m_appManager->scanApp(m_storagePath + "/apps/" + id);
    if (uuid.isNull()) {
        qWarning() << "Error scanning downloaded app. Won't install on watch";
        return;
    }
    m_blobDB->insertAppMetaData(m_appManager->info(uuid));
}

void Pebble::muteNotificationSource(const QString &source)
{
    setNotificationFilter(source, false);
}

void Pebble::resetPebble()
{
    clearTimeline();
    syncCalendar(Core::instance()->platform()->organizerItems());

    clearAppDB();
    syncApps();
}

void Pebble::syncApps()
{
    foreach (const QUuid &appUuid, m_appManager->appUuids()) {
        if (!m_appManager->info(appUuid).isSystemApp()) {
            qDebug() << "Inserting app" << m_appManager->info(appUuid).shortName() << "into BlobDB";
            m_blobDB->insertAppMetaData(m_appManager->info(appUuid));
        }
    }
    // make sure the order is synced too
    m_appManager->setAppOrder(m_appManager->appUuids());
}

void Pebble::syncTime()
{
    QByteArray res;
    QDateTime UTC(QDateTime::currentDateTimeUtc());
    QDateTime local(UTC.toLocalTime());
    local.setTimeSpec(Qt::UTC);
    int offset = UTC.secsTo(local);
    uint val = (local.toMSecsSinceEpoch() + offset) / 1000;

    res.append(0x02); //SET_TIME_REQ
    res.append((char)((val >> 24) & 0xff));
    res.append((char)((val >> 16) & 0xff));
    res.append((char)((val >> 8) & 0xff));
    res.append((char)(val & 0xff));
    m_connection->writeToPebble(WatchConnection::EndpointTime, res);
}

void Pebble::slotUpdateAvailableChanged()
{
    qDebug() << "update available" << m_firmwareDownloader->updateAvailable() << m_firmwareDownloader->candidateVersion();

//    if (m_recovery) {
//        m_firmwareDownloader->performUpgrade();
//    }
    emit updateAvailableChanged();
}

