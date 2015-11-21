#ifndef PLATFORMINTERFACE_H
#define PLATFORMINTERFACE_H

#include "libpebble/pebble.h"
#include "libpebble/musicmetadata.h"

#include <QObject>

class PlatformInterface: public QObject
{
    Q_OBJECT
public:
    PlatformInterface(QObject *parent = 0): QObject(parent) {}
    virtual ~PlatformInterface() {}

// Notifications
signals:
    void notificationReceived(const QString &source, Pebble::NotificationType type, const QString &from, const QString &text, const QString &subject);

// Music
public:
    virtual void sendMusicControlComand(Pebble::MusicControl controlButton) = 0;
    virtual MusicMetaData musicMetaData() const = 0;
signals:
    void musicMetadataChanged(MusicMetaData metaData);

// Phone calls
signals:
    void incomingCall(uint cookie, const QString &number, const QString &name);
    void callStarted(uint cookie);
    void callEnded(uint cookie, bool missed);
public:
    virtual void hangupCall(uint cookie) = 0;
};

#endif // PLATFORMINTERFACE_H

