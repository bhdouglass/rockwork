#include "bluezclient.h"
#include "dbus-shared.h"
#include "device.h"

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
                qDebug() << "have device" << candidatePath;

                auto properties = ifaces.value(BLUEZ_DEVICE_IFACE);
                addDevice(path, properties);
            }
        }
    }
}

QHash<QString, QString> BluezClient::pairedPebbles() const
{
    if (m_bluezManager.isValid()) {
        return m_devices;
    }

    // Try with bluez 4
    QHash<QString, QString> result;

    QDBusConnection system = QDBusConnection::systemBus();

    QDBusReply<QList<QDBusObjectPath> > listAdaptersReply = system.call(
                QDBusMessage::createMethodCall("org.bluez", "/", "org.bluez.Manager",
                                               "ListAdapters"));
    if (!listAdaptersReply.isValid()) {
        qWarning() << listAdaptersReply.error().message();
        return result;
    }

    QList<QDBusObjectPath> adapters = listAdaptersReply.value();

    if (adapters.isEmpty()) {
        qWarning() << "No BT adapters found";
        return result;
    }

    QDBusReply<QVariantMap> adapterPropertiesReply = system.call(
                QDBusMessage::createMethodCall("org.bluez", adapters[0].path(), "org.bluez.Adapter",
                                               "GetProperties"));
    if (!adapterPropertiesReply.isValid()) {
        qWarning() << adapterPropertiesReply.error().message();
        return result;
    }

    QList<QDBusObjectPath> devices;
    adapterPropertiesReply.value()["Devices"].value<QDBusArgument>() >> devices;

    foreach (QDBusObjectPath path, devices) {
        QDBusReply<QVariantMap> devicePropertiesReply = system.call(
                    QDBusMessage::createMethodCall("org.bluez", path.path(), "org.bluez.Device",
                                                   "GetProperties"));
        if (!devicePropertiesReply.isValid()) {
            qCritical() << devicePropertiesReply.error().message();
            continue;
        }

        const QVariantMap &dict = devicePropertiesReply.value();

        QString name = dict["Name"].toString();
        if (name.startsWith("Pebble Time")) {
            qDebug() << "Found Pebble:" << name;
            result.insert(dict["Address"].toString(), name);
        }
    }
    return result;
}

void BluezClient::addDevice(const QDBusObjectPath &path, const QVariantMap &properties)
{
    QString address = properties.value("Address").toString();
    QString name = properties.value("Name").toString();
    if (name.startsWith("Pebble") && !name.startsWith("Pebble Time LE") && !m_devices.contains(address)) {
        qDebug() << "Found new Pebble:" << address << name;
        m_devices.insert(address, name);
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

void BluezClient::slotDevicePairingDone(bool success)
{
    qDebug() << "pairing done" << success;
    if (!success) {
        return;
    }

    Device *device = static_cast<Device*>(sender());
    device->deleteLater();

    if (!m_devices.contains(device->getAddress())) {
        m_devices.insert(device->getAddress(), device->getName());
        emit devicesChanged();
    }
}
