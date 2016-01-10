#include "dbusinterface.h"
#include "core.h"
#include "pebblemanager.h"

DBusPebble::DBusPebble(Pebble *pebble, QObject *parent):
    QObject(parent),
    m_pebble(pebble)
{
    connect(pebble, &Pebble::pebbleConnected, this, &DBusPebble::Connected);
    connect(pebble, &Pebble::pebbleDisconnected, this, &DBusPebble::Disconnected);
    connect(pebble, &Pebble::installedAppsChanged, this, &DBusPebble::InstalledAppsChanged);
    connect(pebble, &Pebble::openURL, this, &DBusPebble::OpenURL);
    connect(pebble, &Pebble::notificationFilterChanged, this, &DBusPebble::NotificationFilterChanged);
    connect(pebble, &Pebble::screenshotAdded, this, &DBusPebble::ScreenshotAdded);
    connect(pebble, &Pebble::screenshotRemoved, this, &DBusPebble::ScreenshotRemoved);
}

QString DBusPebble::Address() const
{
    return m_pebble->address().toString();
}

QString DBusPebble::Name() const
{
    return m_pebble->name();
}

bool DBusPebble::IsConnected() const
{
    return m_pebble->connected();
}

QVariantMap DBusPebble::NotificationsFilter() const
{
    QVariantMap ret;
    QHash<QString, bool> filter = m_pebble->notificationsFilter();
    foreach (const QString &sourceId, filter.keys()) {
        ret.insert(sourceId, filter.value(sourceId));
    }
    return ret;
}

void DBusPebble::SetNotificationFilter(const QString &sourceId, bool enabled)
{
    m_pebble->setNotificationFilter(sourceId, enabled);
}

void DBusPebble::InstallApp(const QString &id)
{
    qDebug() << "installapp called" << id;
    m_pebble->installApp(id);
}

QStringList DBusPebble::InstalledAppIds() const
{
    QStringList ret;
    foreach (const QUuid &id, m_pebble->installedAppIds()) {
        ret << id.toString();
    }
    return ret;
}

QVariantList DBusPebble::InstalledApps() const
{
    QVariantList list;
    foreach (const QUuid &appId, m_pebble->installedAppIds()) {
        QVariantMap app;
        AppInfo info = m_pebble->appInfo(appId);
        app.insert("storeId", info.storeId());
        app.insert("name", info.shortName());
        app.insert("vendor", info.companyName());
        app.insert("watchface", info.isWatchface());
        app.insert("version", info.versionLabel());
        app.insert("uuid", info.uuid().toString());
        app.insert("hasSettings", info.hasSettings());
        app.insert("icon", info.path() + "/list_image.png");
        app.insert("systemApp", info.isSystemApp());

        list.append(app);
    }
    return list;
}

void DBusPebble::RemoveApp(const QString &id)
{
    m_pebble->removeApp(id);
}

void DBusPebble::ConfigurationURL(const QString &uuid)
{
    m_pebble->requestConfigurationURL(QUuid(uuid));
}

void DBusPebble::ConfigurationClosed(const QString &uuid, const QString &result)
{
    m_pebble->configurationClosed(QUuid(uuid), result);
}

void DBusPebble::SetAppOrder(const QStringList &newList)
{
    QList<QUuid> uuidList;
    foreach (const QString &id, newList) {
        uuidList << QUuid(id);
    }
    m_pebble->setAppOrder(uuidList);
}

void DBusPebble::LaunchApp(const QString &uuid)
{
    m_pebble->launchApp(QUuid(uuid));
}

void DBusPebble::RequestScreenshot()
{
    m_pebble->requestScreenshot();
}

QStringList DBusPebble::Screenshots() const
{
    return m_pebble->screenshots();
}

void DBusPebble::RemoveScreenshot(const QString &filename)
{
    qDebug() << "Should remove screenshot" << filename;
    m_pebble->removeScreenshot(filename);
}

QString DBusPebble::SerialNumber() const
{
    return m_pebble->serialNumber();
}

QString DBusPebble::HardwarePlatform() const
{
    switch (m_pebble->hardwarePlatform()) {
    case HardwarePlatformAplite:
        return "aplite";
    case HardwarePlatformBasalt:
        return "basalt";
    case HardwarePlatformChalk:
        return "chalk";
    default:
        ;
    }
    return "unknown";
}

DBusInterface::DBusInterface(QObject *parent) :
    QObject(parent)
{
    QDBusConnection::sessionBus().registerService("org.rockwork");
    QDBusConnection::sessionBus().registerObject("/org/rockwork/Manager", this, QDBusConnection::ExportScriptableSlots|QDBusConnection::ExportScriptableSignals);

    foreach (Pebble *pebble, Core::instance()->pebbleManager()->pebbles()) {
        pebbleAdded(pebble);
    }

    connect(Core::instance()->pebbleManager(), &PebbleManager::pebbleAdded, this, [this](){emit PebblesChanged();});
}

QString DBusInterface::Version()
{
    return QStringLiteral(VERSION);
}

QList<QDBusObjectPath> DBusInterface::ListWatches()
{
    QList<QDBusObjectPath> ret;
    foreach (const QString &address, m_dbusPebbles.keys()) {
        ret.append(QDBusObjectPath("/org/rockwork/" + address));
    }
    return ret;
}

void DBusInterface::pebbleAdded(Pebble *pebble)
{
    QString address = pebble->address().toString().replace(":", "_");
    if (m_dbusPebbles.contains(address)) {
        return;
    }

    DBusPebble *dbusPebble = new DBusPebble(pebble, this);
    m_dbusPebbles.insert(address, dbusPebble);
    QDBusConnection::sessionBus().registerObject("/org/rockwork/" + address, dbusPebble, QDBusConnection::ExportAllContents);
}
