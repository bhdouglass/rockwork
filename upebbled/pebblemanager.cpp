#include "pebblemanager.h"

#include "core.h"
#include "notificationmanager.h"

#include "libpebble/platforminterface.h"

#include <QHash>

#ifdef ENABLE_TESTING
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#endif

PebbleManager::PebbleManager(QObject *parent) : QObject(parent)
{
    m_bluezClient = new BluezClient(this);
    connect(m_bluezClient, &BluezClient::devicesChanged, this, &PebbleManager::loadPebbles);
    loadPebbles();
}

void PebbleManager::loadPebbles()
{
    QHash<QString, QString> pairedPebbles = m_bluezClient->pairedPebbles();
    foreach (const QString &addrString, pairedPebbles.keys()) {
        QBluetoothAddress addr(addrString);
        Pebble *pebble = get(addr);
        if (!pebble) {
            pebble = new Pebble(this);
            pebble->setAddress(addr);
            pebble->setName(pairedPebbles.value(addrString));
            setupPebble(pebble);
            m_pebbles.append(pebble);
        }
        if (!pebble->connected()) {
            pebble->connect();
        }
    }
}

void PebbleManager::setupPebble(Pebble *pebble)
{
    connect(Core::instance()->notificationManager(), &NotificationManager::displayNotification, pebble, &Pebble::sendNotification);

    pebble->setMusicMetadata(Core::instance()->platform()->musicMetaData());
    connect(pebble, &Pebble::musicControlPressed, Core::instance()->platform(), &PlatformInterface::sendMusicControlCommand);
    connect(Core::instance()->platform(), &PlatformInterface::musicMetadataChanged, pebble, &Pebble::setMusicMetadata);

    connect(Core::instance()->platform(), &PlatformInterface::incomingCall, pebble, &Pebble::incomingCall);
    connect(Core::instance()->platform(), &PlatformInterface::callStarted, pebble, &Pebble::callStarted);
    connect(Core::instance()->platform(), &PlatformInterface::callEnded, pebble, &Pebble::callEnded);
    connect(pebble, &Pebble::hangupCall, Core::instance()->platform(), &PlatformInterface::hangupCall);

#ifdef ENABLE_TESTING
    qmlRegisterUncreatableType<Pebble>("PebbleTest", 1, 0, "Pebble", "Dont");
    QQuickView *view = new QQuickView();
    view->engine()->rootContext()->setContextProperty("pebble", pebble);
    view->setSource(QUrl("qrc:///testui/PebbleController.qml"));
    view->show();
#endif
}

Pebble* PebbleManager::get(const QBluetoothAddress &address)
{
    for (int i = 0; i < m_pebbles.count(); i++) {
        if (m_pebbles.at(i)->address() == address) {
            return m_pebbles.at(i);
        }
    }
    return nullptr;
}
