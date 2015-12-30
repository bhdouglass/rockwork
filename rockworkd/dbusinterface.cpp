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
        app.insert("name", info.shortName());
        app.insert("vendor", info.companyName());
        app.insert("watchface", info.isWatchface());
        app.insert("version", info.versionLabel());
        app.insert("uuid", info.uuid().toString());

        list.append(app);
    }
    return list;
}

void DBusPebble::RemoveApp(const QString &id)
{
    m_pebble->removeApp(id);
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
    return QString::number(VERSION);
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



