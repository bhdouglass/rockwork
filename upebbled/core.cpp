#include "core.h"

#include "pebblemanager.h"
#include "notificationmanager.h"

#include "platformintegration/ubuntu/ubuntuplatform.h"
#ifdef ENABLE_TESTING
#include "platformintegration/testing/testingplatform.h"
#endif

#include <QDebug>

Core* Core::s_instance = nullptr;

Core *Core::instance()
{
    if (!s_instance) {
        s_instance = new Core();
    }
    return s_instance;
}

NotificationManager *Core::notificationManager()
{
    return m_notificationManager;
}

PebbleManager *Core::pebbleManager()
{
    return m_pebbleManager;
}

PlatformInterface *Core::platform()
{
    return m_platform;
}

Core::Core(QObject *parent):
    QObject(parent)
{
}

void Core::init()
{
    m_notificationManager = new NotificationManager(this);

    // Platform integration
#ifdef ENABLE_TESTING
    m_platform = new TestingPlatform(this);
#else
    m_platform = new UbuntuPlatform(this);
#endif
    connect(m_platform, &PlatformInterface::notificationReceived, m_notificationManager, &NotificationManager::injectNotification);

    m_pebbleManager = new PebbleManager(this);
}

