#ifndef SCREENSHOTENDPOINT_H
#define SCREENSHOTENDPOINT_H

#include <QObject>

#include "watchconnection.h"

class ScreenshotRequestPackage: public PebblePacket
{
public:
    QByteArray serialize() const override;
private:
    quint8 m_command = 0x00;
};

class ScreenshotEndpoint : public QObject
{
    Q_OBJECT
public:
    enum ResponseCode {
        ResponseCodeOK = 0,
        ResponseCodeMalformedCommand = 1,
        ResponseCodeOutOfMemory = 2,
        ResponseCodeAlreadyInProgress = 3
    };

    explicit ScreenshotEndpoint(WatchConnection *connection, QObject *parent = 0);

    void requestScreenshot();

signals:
    void screenshotSaved(const QString &filename);

private slots:
    void handleScreenshotData(const QByteArray &data);

private:
    WatchConnection *m_connection;
    quint32 m_waitingForMore = 0;
    quint32 m_version = 0;
    quint32 m_width = 0;
    quint32 m_height = 0;
    QByteArray m_accumulatedData;
};

#endif // SCREENSHOTENDPOINT_H
