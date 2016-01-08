#ifndef PEBBLE_H
#define PEBBLE_H

#include <QObject>
#include <QDBusInterface>

class ApplicationsModel;

class Pebble : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString hardwarePlatform READ hardwarePlatform NOTIFY hardwarePlatformChanged)
    Q_PROPERTY(ApplicationsModel* installedApps READ installedApps CONSTANT)
    Q_PROPERTY(ApplicationsModel* installedWatchfaces READ installedWatchfaces CONSTANT)
public:
    explicit Pebble(const QDBusObjectPath &path, QObject *parent = 0);

    QDBusObjectPath path();

    bool connected() const;
    QString address() const;
    QString name() const;
    QString hardwarePlatform() const;
    QString serialNumber() const;

    ApplicationsModel* installedApps() const;
    ApplicationsModel* installedWatchfaces() const;

public slots:
    void removeApp(const QString &uuid);
    void installApp(const QString &storeId);
    void moveApp(const QString &uuid, int toIndex);
    void requestConfigurationURL(const QString &uuid);
    void configurationClosed(const QString &uuid, const QString &url);
    void launchApp(const QString &uuid);
    void requestScreenshot();

signals:
    void connectedChanged();
    void hardwarePlatformChanged();

    void openURL(const QString &uuid, const QString &url);

private:
    QVariant fetchProperty(const QString &propertyName);

private slots:
    void dataChanged();
    void pebbleConnected();
    void pebbleDisconnected();
    void refreshApps();
    void appsSorted();

private:
    QDBusObjectPath m_path;

    bool m_connected = false;
    QString m_address;
    QString m_name;
    QString m_hardwarePlatform;
    QString m_serialNumber;
    QDBusInterface *m_iface;
    ApplicationsModel *m_installedApps;
    ApplicationsModel *m_installedWatchfaces;
};

#endif // PEBBLE_H
