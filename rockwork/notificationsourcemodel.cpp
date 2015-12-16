#include "notificationsourcemodel.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

NotificationSourceModel::NotificationSourceModel(QObject *parent) : QAbstractListModel(parent)
{
    loadSources();
}

int NotificationSourceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_sources.count();
}

QVariant NotificationSourceModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleName:
        return m_sources.keys().at(index.row());
    case RoleEnabled:
        return m_sources.values().at(index.row());
    }
    return QVariant();
}

QHash<int, QByteArray> NotificationSourceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleName, "name");
    roles.insert(RoleEnabled, "enabled");
    return roles;
}

void NotificationSourceModel::setEnabled(int index, bool enabled)
{
    QString settingsFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/rockwork.mzanetti/notifications.conf";
    qDebug() << "Settings file" << settingsFile;
    QSettings settings(settingsFile, QSettings::IniFormat);
    QString source = m_sources.keys().at(index);
    m_sources[source] = enabled;
    settings.setValue(source, enabled);
    emit dataChanged(this->index(index), this->index(index));

}

void NotificationSourceModel::loadSources()
{
    QString settingsFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/rockwork.mzanetti/notifications.conf";
    qDebug() << "Settings file" << settingsFile;
    QSettings settings(settingsFile, QSettings::IniFormat);

    beginResetModel();
    m_sources.clear();
    foreach (const QString &source, settings.allKeys()) {
        m_sources.insert(source, settings.value(source).toBool());
    }
    endResetModel();
}

