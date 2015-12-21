#include "applicationsmodel.h"

#include <QDebug>

ApplicationsModel::ApplicationsModel(QObject *parent):
    QAbstractListModel(parent)
{
}

int ApplicationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_apps.count();
}

QVariant ApplicationsModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleId:
        return m_apps.at(index.row())->id();
    case RoleName:
        return m_apps.at(index.row())->name();
    case RoleIcon:
        return m_apps.at(index.row())->icon();
    case RoleVendor:
        return m_apps.at(index.row())->vendor();
    case RoleVersion:
        return m_apps.at(index.row())->version();
    }
    return QVariant();
}

QHash<int, QByteArray> ApplicationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleId, "id");
    roles.insert(RoleName, "name");
    roles.insert(RoleIcon, "icon");
    roles.insert(RoleVendor, "vendor");
    roles.insert(RoleVersion, "version");
    return roles;
}

void ApplicationsModel::clear()
{
    beginResetModel();
    qDeleteAll(m_apps);
    m_apps.clear();
    endResetModel();
}

void ApplicationsModel::insert(AppItem *item)
{
    item->setParent(this);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_apps.append(item);
    endInsertRows();
}

AppItem *ApplicationsModel::get(int index) const
{
    return m_apps.at(index);
}

AppItem::AppItem(QObject *parent):
    QObject(parent)
{

}

QString AppItem::id() const
{
    return m_id;
}

QString AppItem::name() const
{
    return m_name;
}

QString AppItem::icon() const
{
    return m_icon;
}

QString AppItem::vendor() const
{
    return m_vendor;
}

QString AppItem::version() const
{
    return m_version;
}

bool AppItem::isWatchFace() const
{
    return m_isWatchFace;
}

void AppItem::setId(const QString &id)
{
    m_id = id;
}

void AppItem::setName(const QString &name)
{
    m_name = name;
}

void AppItem::setIcon(const QString &icon)
{
    m_icon = icon;
}

void AppItem::setVendor(const QString &vendor)
{
    m_vendor = vendor;
}

void AppItem::setVersion(const QString &version)
{
    m_version = version;
}

void AppItem::setIsWatchFace(bool isWatchFace)
{
    m_isWatchFace = isWatchFace;
}
