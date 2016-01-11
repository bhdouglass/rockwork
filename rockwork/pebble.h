#ifndef PEBBLE_H
#define PEBBLE_H

#include <QObject>
#include <QDBusInterface>

class NotificationSourceModel;
class ApplicationsModel;
class ScreenshotModel;

class Pebble : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString hardwarePlatform READ hardwarePlatform NOTIFY hardwarePlatformChanged)
    Q_PROPERTY(int model READ model NOTIFY modelChanged)
    Q_PROPERTY(NotificationSourceModel* notifications READ notifications CONSTANT)
    Q_PROPERTY(ApplicationsModel* installedApps READ installedApps CONSTANT)
    Q_PROPERTY(ApplicationsModel* installedWatchfaces READ installedWatchfaces CONSTANT)
    Q_PROPERTY(ScreenshotModel* screenshots READ screenshots CONSTANT)
public:
    explicit Pebble(const QDBusObjectPath &path, QObject *parent = 0);

    QDBusObjectPath path();

    bool connected() const;
    QString address() const;
    QString name() const;
    QString hardwarePlatform() const;
    QString serialNumber() const;
    int model() const;

    NotificationSourceModel *notifications() const;
    ApplicationsModel* installedApps() const;
    ApplicationsModel* installedWatchfaces() const;
    ScreenshotModel* screenshots() const;

public slots:
    void setNotificationFilter(const QString &sourceId, bool enabled);
    void removeApp(const QString &uuid);
    void installApp(const QString &storeId);
    void moveApp(const QString &uuid, int toIndex);
    void requestConfigurationURL(const QString &uuid);
    void configurationClosed(const QString &uuid, const QString &url);
    void launchApp(const QString &uuid);
    void requestScreenshot();
    void removeScreenshot(const QString &filename);

signals:
    void connectedChanged();
    void hardwarePlatformChanged();
    void modelChanged();

    void openURL(const QString &uuid, const QString &url);

private:
    QVariant fetchProperty(const QString &propertyName);

private slots:
    void dataChanged();
    void pebbleConnected();
    void pebbleDisconnected();
    void notificationFilterChanged(const QString &sourceId, bool enabled);
    void refreshNotifications();
    void refreshApps();
    void appsSorted();
    void refreshScreenshots();
    void screenshotAdded(const QString &filename);
    void screenshotRemoved(const QString &filename);

private:
    QDBusObjectPath m_path;

    bool m_connected = false;
    QString m_address;
    QString m_name;
    QString m_hardwarePlatform;
    QString m_serialNumber;
    int m_model = 0;
    QDBusInterface *m_iface;
    NotificationSourceModel *m_notifications;
    ApplicationsModel *m_installedApps;
    ApplicationsModel *m_installedWatchfaces;
    ScreenshotModel *m_screenshotModel;
};

#endif // PEBBLE_H
