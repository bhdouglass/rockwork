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

    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "Connected", this, SLOT(pebbleConnected()));
    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "Disconnected", this, SLOT(pebbleDisconnected()));
    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "InstalledAppsChanged", this, SLOT(refreshApps()));

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

QString Pebble::serialNumber() const
{
    return m_serialNumber;
}

ApplicationsModel *Pebble::installedApps() const
{
    return m_installedApps;
}

void Pebble::removeApp(const QString &id)
{
    qDebug() << "should remove app" << id;
    m_iface->call("RemoveApp", id);
}

void Pebble::requestConfigurationURL(const QString &id)
{
    qDebug() << "requesting settings url" << id;
    m_iface->call("ConfigurationURL", id);
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
}

void Pebble::pebbleDisconnected()
{
    m_connected = false;
    emit connectedChanged();
}

void Pebble::refreshApps()
{

    QDBusMessage m = m_iface->call("InstalledApps");
    if (m.type() == QDBusMessage::ErrorMessage || m.arguments().count() == 0) {
        qWarning() << "Could not fetch installed apps" << m.errorMessage();
        return;
    }

    m_installedApps->clear();

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
        qDebug() << v.toMap().value("id").toString() << v.toMap().value("hasSettings").toBool();
        AppItem *app = new AppItem(this);
        app->setId(v.toMap().value("id").toString());
        app->setName(v.toMap().value("name").toString());
        app->setIcon(v.toMap().value("icon").toString());
        app->setVendor(v.toMap().value("vendor").toString());
        app->setVersion(v.toMap().value("version").toString());
        app->setIsWatchFace(v.toMap().value("watchface").toBool());
        app->setHasSettings(v.toMap().value("hasSettings").toBool());
        m_installedApps->insert(app);
    }
}
