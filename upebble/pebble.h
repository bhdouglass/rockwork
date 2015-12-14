#ifndef PEBBLE_H
#define PEBBLE_H

#include <QObject>
#include <QDBusInterface>

class Pebble : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
public:
    explicit Pebble(const QDBusObjectPath &path, QObject *parent = 0);

    QDBusObjectPath path();

    bool connected() const;
    QString address() const;
    QString name() const;
    QString serialNumber() const;

private:
    QVariant fetchProperty(const QString &propertyName);

private slots:
    void dataChanged();
    void pebbleConnected();
    void pebbleDisconnected();

signals:
    void connectedChanged();

private:
    QDBusObjectPath m_path;

    bool m_connected = false;
    QString m_address;
    QString m_name;
    QString m_serialNumber;
    QDBusInterface *m_iface;
};

#endif // PEBBLE_H
