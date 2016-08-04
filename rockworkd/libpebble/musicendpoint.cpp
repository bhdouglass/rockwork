#include "musicendpoint.h"
#include "pebble.h"
#include "watchconnection.h"
#include "watchdatawriter.h"

#include <QDebug>

MusicEndpoint::MusicEndpoint(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_watchConnection(connection)
{
    m_watchConnection->registerEndpointHandler(WatchConnection::EndpointMusicControl, this, "handleMessage");
}

void MusicEndpoint::setMusicMetadata(const MusicMetaData &metaData)
{
    if (m_watchConnection->isConnected() && metaData != m_metaData) {
        m_metaData = metaData;
        writeMetadata();
    }
}

void MusicEndpoint::setPlayState(const MusicPlayState &playState)
{
    if (m_watchConnection->isConnected() && playState != m_playState) {
        m_playState = playState;
        writePlayState();
    }
}

void MusicEndpoint::writeMetadata()
{
    qDebug() << "Writing metadata to watch";
    QStringList tmp;
    tmp.append(m_metaData.artist.left(30));
    tmp.append(m_metaData.album.left(30));
    tmp.append(m_metaData.title.left(30));
    QByteArray res = m_watchConnection->buildMessageData(MusicControlUpdateCurrentTrack, tmp);

    WatchDataWriter writer(&res);
    writer.writeLE(m_metaData.length);
    writer.writeLE(m_metaData.trackCount);
    writer.writeLE(m_metaData.currentTrack);

    m_watchConnection->writeToPebble(WatchConnection::EndpointMusicControl, res);
}


void MusicEndpoint::writePlayState() {
    if (!m_watchConnection->isConnected()) {
        return;
    }
    QByteArray res;
    WatchDataWriter writer(&res);
    res.append(MusicControlUpdatePlayStateInfo);
    res.append(m_playState.state);
    writer.writeLE(m_playState.trackPosition);
    writer.writeLE(m_playState.playRate);
    res.append(m_playState.shuffle);
    res.append(m_playState.repeat);

    qDebug() << "writing play state to pebble" << res.toHex();
    m_watchConnection->writeToPebble(WatchConnection::EndpointMusicControl, res);
}
void MusicEndpoint::handleMessage(const QByteArray &data)
{
    MusicControlCommand controlCommand = (MusicControlCommand)data.toHex().toInt();
    switch (controlCommand) {
    case MusicControlPlayPause:
    case MusicControlPause:
    case MusicControlPlay:
    case MusicControlNextTrack:
    case MusicControlPreviousTrack:
    case MusicControlVolumeUp:
    case MusicControlVolumeDown:
        emit musicControlPressed(controlCommand);
        break;
    case MusicControlGetCurrentTrack:
        writeMetadata();
        writePlayState();
        return;
    default:
        qWarning() << "Unhandled music control command:" << data.toHex();
        return;
    }
}

