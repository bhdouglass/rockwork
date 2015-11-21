#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>

class PebbleManager;
class NotificationManager;
class PlatformInterface;

class Core : public QObject
{
    Q_OBJECT
public:
    static Core *instance();

    NotificationManager* notificationManager();
    PebbleManager* pebbleManager();
    PlatformInterface* platform();

    void init();
private:
    explicit Core(QObject *parent = 0);
    static Core *s_instance;

private slots:

private:
    PebbleManager *m_pebbleManager;
    NotificationManager *m_notificationManager;
    PlatformInterface *m_platform;
};

#endif // CORE_H
