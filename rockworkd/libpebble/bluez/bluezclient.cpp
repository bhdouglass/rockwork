#include "bluezclient.h"
#include "dbus-shared.h"

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>

BluezClient::BluezClient(QObject *parent):
    QObject(parent),
    m_dbus(QDBusConnection::systemBus()),
    m_bluezManager("org.bluez", "/", m_dbus),
    m_bluezAgentManager("org.bluez", "/org/bluez", m_dbus)
{
    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    if (m_bluezManager.isValid()) {
        connect(&m_bluezManager, SIGNAL(InterfacesAdded(const QDBusObjectPath&, InterfaceList)),
                this, SLOT(slotInterfacesAdded(const QDBusObjectPath&, InterfaceList)));

        connect(&m_bluezManager, SIGNAL(InterfacesRemoved(const QDBusObjectPath&, const QStringList&)),
                this, SLOT(slotInterfacesRemoved(const QDBusObjectPath&, const QStringList&)));

        auto objectList = m_bluezManager.GetManagedObjects().argumentAt<0>();
        for (QDBusObjectPath path : objectList.keys()) {
            InterfaceList ifaces = objectList.value(path);
            if (ifaces.contains(BLUEZ_DEVICE_IFACE)) {
                QString candidatePath = path.path();

                auto properties = ifaces.value(BLUEZ_DEVICE_IFACE);
                addDevice(path, properties);
            }
        }
    }
}

QList<Device> BluezClient::pairedPebbles() const
{
    QList<Device> ret;
    if (m_bluezManager.isValid()) {
        foreach (const Device &dev, m_devices) {
            ret << dev;
        }
    }
    return ret;
}

void BluezClient::addDevice(const QDBusObjectPath &path, const QVariantMap &properties)
{
    QString address = properties.value("Address").toString();
    QString name = properties.value("Name").toString();
    if (name.startsWith("Pebble") && !name.startsWith("Pebble Time LE") && !m_devices.contains(address)) {
        qDebug() << "Found new Pebble:" << address << name;
        Device device;
        device.address = QBluetoothAddress(address);
        device.name = name;
        device.path = path.path();
        m_devices.insert(path.path(), device);
        qDebug() << "emitting added";
        emit devicesChanged();
    }
}

void BluezClient::slotInterfacesAdded(const QDBusObjectPath &path, InterfaceList ifaces)
{
    qDebug() << "Interface added!";
    if (ifaces.contains(BLUEZ_DEVICE_IFACE)) {
        auto properties = ifaces.value(BLUEZ_DEVICE_IFACE);
        addDevice(path, properties);
    }
}

void BluezClient::slotInterfacesRemoved(const QDBusObjectPath &path, const QStringList &ifaces)
{
    qDebug() << "interfaces removed" << path.path() << ifaces;
    if (!ifaces.contains(BLUEZ_DEVICE_IFACE)) {
        return;
    }
    if (m_devices.contains(path.path())) {
        m_devices.take(path.path());
        qDebug() << "removing dev";
        emit devicesChanged();
    }
}
