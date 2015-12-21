#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QHash>
#include <QUuid>
#include "appinfo.h"
#include "watchconnection.h"

class Pebble;

class AppFetchResponse: public PebblePacket
{
public:
    enum Status {
        StatusStart = 0x01,
        StatusBusy = 0x02,
        StatusInvalidUUID = 0x03,
        StatusNoData = 0x04
    };
    AppFetchResponse(Status status = StatusNoData);
    void setStatus(Status status);

    QByteArray serialize() const override;

private:
    quint8 m_command = 1; // I guess there's only one command for now
    Status m_status = StatusNoData;
};

class AppManager : public QObject
{
    Q_OBJECT

public:
    enum Action {
        ActionGetAppBankStatus = 1,
        ActionRemoveApp = 2,
        ActionRefreshApp = 3,
        ActionGetAppBankUuids = 5
    };

    explicit AppManager(Pebble *pebble, WatchConnection *connection);

    QStringList appPaths() const;
    QList<QUuid> appUuids() const;
    QList<QString> appIds() const;

    AppInfo info(const QUuid &uuid) const;
    AppInfo info(const QString &appid) const;

    void insertAppInfo(const AppInfo &info);

    void scanApp(const QString &path);

    void removeApp(const QString &id);

public slots:
    void rescan();

    void handleAppFetchMessage(const QByteArray &data);

signals:
    void appsChanged();

    void uploadRequested(const QString &file, quint32 appInstallId);

private:

private:
    Pebble *m_pebble;
    WatchConnection *m_connection;
    QHash<QUuid, AppInfo> m_appsUuids;
    QHash<QString, QUuid> m_appsIds;
};

#endif // APPMANAGER_H
