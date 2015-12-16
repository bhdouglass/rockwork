#include "pebble.h"

#include <QDebug>

Pebble::Pebble(const QDBusObjectPath &path, QObject *parent):
    QObject(parent),
    m_path(path)
{
    m_iface = new QDBusInterface("org.rockwork", path.path(), "org.rockwork.Pebble", QDBusConnection::sessionBus(), this);

    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "Connected", this, SLOT(pebbleConnected()));
    QDBusConnection::sessionBus().connect("org.rockwork", path.path(), "org.rockwork.Pebble", "Disconnected", this, SLOT(pebbleDisconnected()));
    dataChanged();
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
