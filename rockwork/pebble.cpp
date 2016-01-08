#include "pebble.h"
#include "applicationsmodel.h"

#include <QDBusArgument>
#include <QDebug>

Pebble::Pebble(const QDBusObjectPath &path, QObject *parent):
    QObject(parent),
    m_path(path)
{
    m_iface = new QDBusInterface("org.rockwork", path.path(), "org.rockwork.Pebble", QDBusConnection::sessionBus(), this);
    m_installedApps = new ApplicationsModel(this);
    connect(m_installedApps, &ApplicationsModel::appsSorted, this, &Pebble::appsSorted);
    m_installedWatchfaces = new ApplicationsModel(this);

    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "Connected", this, SLOT(pebbleConnected()));
    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "Disconnected", this, SLOT(pebbleDisconnected()));
    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "InstalledAppsChanged", this, SLOT(refreshApps()));
    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "OpenURL", this, SIGNAL(openURL(const QString&, const QString&)));

    dataChanged();
    refreshApps();
}

bool Pebble::connected() const
{
    return m_connected;
}

QDBusObjectPath Pebble::path()
{
    return m_path;
}

QString Pebble::address() const
{
    return m_address;
}

QString Pebble::name() const
{
    return m_name;
}

QString Pebble::hardwarePlatform() const
{
    return m_hardwarePlatform;
}

QString Pebble::serialNumber() const
{
    return m_serialNumber;
}

ApplicationsModel *Pebble::installedApps() const
{
    return m_installedApps;
}

ApplicationsModel *Pebble::installedWatchfaces() const
{
    return m_installedWatchfaces;
}

void Pebble::configurationClosed(const QString &uuid, const QString &url)
{
    m_iface->call("ConfigurationClosed", uuid, url.mid(17));
}

void Pebble::launchApp(const QString &uuid)
{
    m_iface->call("LaunchApp", uuid);
}

void Pebble::requestConfigurationURL(const QString &uuid)
{
    m_iface->call("ConfigurationURL", uuid);
}

void Pebble::removeApp(const QString &uuid)
{
    qDebug() << "should remove app" << uuid;
    m_iface->call("RemoveApp", uuid);
}

void Pebble::installApp(const QString &storeId)
{
    qDebug() << "should install app" << storeId;
    m_iface->call("InstallApp", storeId);
}

QVariant Pebble::fetchProperty(const QString &propertyName)
{
    QDBusMessage m = m_iface->call(propertyName);
    if (m.type() != QDBusMessage::ErrorMessage && m.arguments().count() == 1) {
        qDebug() << "property" << propertyName << m.arguments().first();
        return m.arguments().first();

    }
    qDebug() << "error getting property:" << propertyName << m.errorMessage();
    return QVariant();
}

void Pebble::dataChanged()
{
    qDebug() << "data changed";
    m_name = fetchProperty("Name").toString();;
    m_address = fetchProperty("Address").toString();
    m_serialNumber = fetchProperty("SerialNumber").toString();
    QString hardwarePlatform = fetchProperty("HardwarePlatform").toString();
    if (hardwarePlatform != m_hardwarePlatform) {
        m_hardwarePlatform = hardwarePlatform;
        emit hardwarePlatformChanged();
    }

    bool connected = fetchProperty("IsConnected").toBool();
    if (connected != m_connected) {
        m_connected = connected;
        emit connectedChanged();
    }
}

void Pebble::pebbleConnected()
{
    m_connected = true;
    emit connectedChanged();

    dataChanged();
    refreshApps();
}

void Pebble::pebbleDisconnected()
{
    m_connected = false;
    emit connectedChanged();
}

void Pebble::moveApp(const QString &uuid, int toIndex)
{
    // This is a bit tricky:
    AppItem *item = m_installedApps->findByUuid(uuid);
    if (!item) {
        qWarning() << "item not found";
        return;
    }
    int realToIndex = 0;
    for (int i = 0; i < m_installedApps->rowCount(); i++) {
        if (item->isWatchFace() && m_installedApps->get(i)->isWatchFace()) {
            realToIndex++;
        } else if (!item->isWatchFace() && !m_installedApps->get(i)->isWatchFace()) {
            realToIndex++;
        }
        if (realToIndex == toIndex) {
            realToIndex = i+1;
            break;
        }
    }
    m_iface->call("MoveApp", m_installedApps->indexOf(item), realToIndex);
}

void Pebble::refreshApps()
{

    QDBusMessage m = m_iface->call("InstalledApps");
    if (m.type() == QDBusMessage::ErrorMessage || m.arguments().count() == 0) {
        qWarning() << "Could not fetch installed apps" << m.errorMessage();
        return;
    }

    m_installedApps->clear();
    m_installedWatchfaces->clear();

    const QDBusArgument &arg = m.arguments().first().value<QDBusArgument>();

    QVariantList appList;

    arg.beginArray();
    while (!arg.atEnd()) {
        QVariant mapEntryVariant;
        arg >> mapEntryVariant;

        QDBusArgument mapEntry = mapEntryVariant.value<QDBusArgument>();
        QVariantMap appMap;
        mapEntry >> appMap;
        appList.append(appMap);

    }
    arg.endArray();


    qDebug() << "have apps" << appList;
    foreach (const QVariant &v, appList) {
        AppItem *app = new AppItem(this);
        app->setStoreId(v.toMap().value("storeId").toString());
        app->setUuid(v.toMap().value("uuid").toString());
        app->setName(v.toMap().value("name").toString());
        app->setIcon(v.toMap().value("icon").toString());
        app->setVendor(v.toMap().value("vendor").toString());
        app->setVersion(v.toMap().value("version").toString());
        app->setIsWatchFace(v.toMap().value("watchface").toBool());
        app->setHasSettings(v.toMap().value("hasSettings").toBool());
        app->setIsSystemApp(v.toMap().value("systemApp").toBool());

        if (app->isWatchFace()) {
            m_installedWatchfaces->insert(app);
        } else {
            m_installedApps->insert(app);
        }
    }
}

void Pebble::appsSorted()
{
    QStringList newList;
    for (int i = 0; i < m_installedApps->rowCount(); i++) {
        newList << m_installedApps->get(i)->uuid();
    }
    for (int i = 0; i < m_installedWatchfaces->rowCount(); i++) {
        newList << m_installedWatchfaces->get(i)->uuid();
    }
    m_iface->call("SetAppOrder", newList);
}

void Pebble::requestScreenshot()
{
    m_iface->call("RequestScreenshot");
}
