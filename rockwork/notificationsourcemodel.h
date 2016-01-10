#ifndef NOTIFICATIONSOURCEMODEL_H
#define NOTIFICATIONSOURCEMODEL_H

#include <QAbstractListModel>

class NotificationSourceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles {
        RoleName,
        RoleEnabled
    };

    explicit NotificationSourceModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insert(const QString &sourceId, bool enabled);

signals:
    void countChanged();

private:
    QStringList m_sources;
    QHash<QString, bool> m_sourceValues;

};

#endif // NOTIFICATIONSOURCEMODEL_H
