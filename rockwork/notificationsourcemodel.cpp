#include "notificationsourcemodel.h"

#include <QSettings>
#include <QDebug>

NotificationSourceModel::NotificationSourceModel(QObject *parent) : QAbstractListModel(parent)
{
}

int NotificationSourceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_sources.count();
}

QVariant NotificationSourceModel::data(const QModelIndex &index, int role) const
{
    QString sourceId = m_sources.at(index.row());
    switch (role) {
    case RoleName:
        return sourceId;
    case RoleEnabled:
        return m_sourceValues.value(sourceId);
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

void NotificationSourceModel::insert(const QString &sourceId, bool enabled)
{
    qDebug() << "changed" << sourceId << enabled;
    int idx = m_sources.indexOf(sourceId);
    if (idx >= 0) {
        m_sourceValues[sourceId] = enabled;
        emit dataChanged(index(idx), index(idx), {RoleEnabled});
    } else {
        beginInsertRows(QModelIndex(), m_sources.count(), m_sources.count());
        m_sources.append(sourceId);
        m_sourceValues[sourceId] = enabled;
        endInsertRows();
    }
}

