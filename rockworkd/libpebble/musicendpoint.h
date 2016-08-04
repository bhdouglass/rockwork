#ifndef MUSICENDPOINT_H
#define MUSICENDPOINT_H

#include "musicmetadata.h"
#include "enums.h"

#include <QObject>

class Pebble;
class WatchConnection;

class MusicEndpoint : public QObject
{
    Q_OBJECT
public:
    explicit MusicEndpoint(Pebble *pebble, WatchConnection *connection);

public slots:
    void setMusicMetadata(const MusicMetaData &metaData);
    void setPlayState(const MusicPlayState &playState);

private slots:
    void handleMessage(const QByteArray &data);

signals:
    void musicControlPressed(MusicControlCommand button);

private:
    void writeMetadata();
    void writePlayState();

private:
    Pebble *m_pebble;
    WatchConnection *m_watchConnection;

    MusicMetaData m_metaData;
    MusicPlayState m_playState;
};

#endif // MUSICENDPOINT_H
