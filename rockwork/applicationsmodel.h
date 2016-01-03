#ifndef APPLICATIONSMODEL_H
#define APPLICATIONSMODEL_H

#include <QAbstractListModel>
#include <QDBusObjectPath>

class QDBusInterface;

class AppItem: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id MEMBER m_id)
    Q_PROPERTY(QString uuid MEMBER m_uuid)
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QString icon MEMBER m_icon)
    Q_PROPERTY(QString vendor MEMBER m_vendor)
    Q_PROPERTY(QString version MEMBER m_version)
    Q_PROPERTY(bool isWatchFace MEMBER m_isWatchFace)
    Q_PROPERTY(bool hasSettings MEMBER m_hasSettings)

public:
    AppItem(QObject *parent = 0);

    QString id() const;
    QString uuid() const;
    QString name() const;
    QString icon() const;
    QString vendor() const;
    QString version() const;
    bool isWatchFace() const;
    bool hasSettings() const;

    void setId(const QString &id);
    void setUuid(const QString &uuid);
    void setName(const QString &name);
    void setIcon(const QString &icon);
    void setVendor(const QString &vendor);
    void setVersion(const QString &version);
    void setIsWatchFace(bool isWatchFace);
    void setHasSettings(bool hasSettings);

private:
    QString m_id;
    QString m_uuid;
    QString m_name;
    QString m_icon;
    QString m_vendor;
    QString m_version;
    bool m_isWatchFace = false;
    bool m_hasSettings = false;
};

class ApplicationsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        RoleId,
        RoleUuid,
        RoleName,
        RoleIcon,
        RoleVendor,
        RoleVersion,
        RoleIsWatchFace,
        RoleHasSettings
    };

    ApplicationsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void clear();
    void insert(AppItem *item);
    AppItem* get(int index) const;

private:
    QList<AppItem*> m_apps;
};

#endif // APPLICATIONSMODEL_H