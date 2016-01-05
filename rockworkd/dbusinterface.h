#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class Pebble;

class DBusPebble: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.rockwork.Pebble")
public:
    DBusPebble(Pebble *pebble, QObject *parent);

signals:
    void Connected();
    void Disconnected();
    void InstalledAppsChanged();
    void OpenURL(const QString &uuid, const QString &url);
    void ScreenshotSaved(const QString &filename);

public slots:
    QString Address() const;
    QString Name() const;
    QString SerialNumber() const;
    QString HardwarePlatform() const;
    bool IsConnected() const;

    void InstallApp(const QString &id);
    QStringList InstalledAppIds() const;
    QVariantList InstalledApps() const;
    void RemoveApp(const QString &id);
    void ConfigurationURL(const QString &uuid);
    void ConfigurationClosed(const QString &uuid, const QString &result);
    void SetAppOrder(const QStringList &newList);
    void LaunchApp(const QString &uuid);
    void RequestScreenshot();

private:
    Pebble *m_pebble;
};

class DBusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.rockwork.Manager")

public:
    explicit DBusInterface(QObject *parent = 0);

public slots:
    Q_SCRIPTABLE QString Version();
    Q_SCRIPTABLE QList<QDBusObjectPath> ListWatches();

signals:
    Q_SCRIPTABLE void PebblesChanged();
    void NameChanged();

private slots:
    void pebbleAdded(Pebble *pebble);

private:
    QHash<QString, DBusPebble*> m_dbusPebbles;
};

#endif // DBUSINTERFACE_H
