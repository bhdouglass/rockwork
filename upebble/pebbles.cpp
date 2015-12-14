#include "pebbles.h"
#include "pebble.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QDBusArgument>
#include <QDBusServiceWatcher>

Pebbles::Pebbles(QObject *parent):
    QAbstractListModel(parent)
{
    refresh();
    m_watcher = new QDBusServiceWatcher("org.upebble", QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration, this);
    QDBusConnection::sessionBus().connect("org.upebble", "/org/upebble/Manager", "org.upebble.Manager", "PebblesChanged", this, SLOT(refresh()));
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, [this]() {
        refresh();
        QDBusConnection::sessionBus().connect("org.upebble", "/org/upebble/Manager", "org.upebble.Manager", "PebblesChanged", this, SLOT(refresh()));
    });
}

int Pebbles::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_pebbles.count();
}

QVariant Pebbles::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleAddress:
        return m_pebbles.at(index.row())->address();
    case RoleName:
        return m_pebbles.at(index.row())->name();
    case RoleSerialNumber:
        return m_pebbles.at(index.row())->serialNumber();
    case RoleConnected:
        return m_pebbles.at(index.row())->connected();
    }

    return QVariant();
}

QHash<int, QByteArray> Pebbles::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(RoleAddress, "address");
    roles.insert(RoleName, "name");
    roles.insert(RoleSerialNumber, "serialNumber");
    roles.insert(RoleConnected, "connected");
    return roles;
}

QString Pebbles::version() const
{
    QDBusInterface iface("org.upebble", "/org/upebble/Manager", "org.upebble.Manager");
    if (!iface.isValid()) {
        qWarning() << "Could not connect to upebbled.";
        return QString();
    }
    QDBusMessage reply = iface.call("Version");
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Error refreshing watches:" << reply.errorMessage();
        return QString();
    }
    if (reply.arguments().count() == 0) {
        qWarning() << "No reply from service.";
        return QString();
    }
    return reply.arguments().first().toString();
}

Pebble *Pebbles::get(int index) const
{
    return m_pebbles.at(index);
}

int Pebbles::find(const QString &address) const
{
    for (int i = 0; i < m_pebbles.count(); i++) {
        if (m_pebbles.at(i)->address() == address) {
            return i;
        }
    }
    return -1;
}

void Pebbles::refresh()
{
    QDBusInterface iface("org.upebble", "/org/upebble/Manager", "org.upebble.Manager");
    if (!iface.isValid()) {
        qWarning() << "Could not connect to upebbled.";
        return;
    }
    QDBusMessage reply = iface.call("ListWatches");
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Error refreshing watches:" << reply.errorMessage();
        return;
    }
    if (reply.arguments().count() == 0) {
        qWarning() << "No reply from service.";
        return;
    }
    QDBusArgument arg = reply.arguments().first().value<QDBusArgument>();
    arg.beginArray();
    while (!arg.atEnd()) {
        QDBusObjectPath p;
        arg >> p;
        if (find(p) == -1) {
            Pebble *pebble = new Pebble(p, this);
            connect(pebble, &Pebble::connectedChanged, this, &Pebbles::pebbleConnectedChanged);
            beginInsertRows(QModelIndex(), m_pebbles.count(), m_pebbles.count());
            m_pebbles.append(pebble);
            endInsertRows();
            emit countChanged();
        }
    }
    arg.endArray();
}

void Pebbles::pebbleConnectedChanged()
{
    Pebble *pebble = static_cast<Pebble*>(sender());
    emit dataChanged(index(find(pebble->address())), index(find(pebble->address())), {RoleConnected});
}

int Pebbles::find(const QDBusObjectPath &path) const
{
    for (int i = 0; i < m_pebbles.count(); i++) {
        if (m_pebbles.at(i)->path() == path) {
            return i;
        }
    }
    return -1;
}

