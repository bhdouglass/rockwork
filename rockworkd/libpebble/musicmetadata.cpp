#include "musicmetadata.h"

MusicMetaData::MusicMetaData()
{

}

MusicMetaData::MusicMetaData(const QString &artist, const QString &album, const QString &title, qlonglong length):
    artist(artist),
    album(album),
    title(title),
    length(length)
{

}

bool MusicMetaData::operator==(const MusicMetaData &other) const
{
    return artist == other.artist
            && album == other.album
            && title == other.title
            && length == other.length
            && currentTrack == other.currentTrack
            && trackCount == other.trackCount;
}

bool MusicMetaData::operator!=(const MusicMetaData &other) const
{
    return !operator==(other);
}

MusicPlayState::MusicPlayState()
{

}

MusicPlayState::MusicPlayState(const MusicPlayState::State state, const qint32 trackPosition, const qint32 playRate, const MusicPlayState::Shuffle shuffle, const MusicPlayState::Repeat repeat) :
    state(state),
    trackPosition(trackPosition),
    playRate(playRate),
    shuffle(shuffle),
    repeat(repeat)
{

}

bool MusicPlayState::operator==(const MusicPlayState &other) const
{
    return state == other.state
            && trackPosition == other.trackPosition
            && playRate == other.playRate
            && shuffle == other.shuffle
            && repeat == other.repeat;
}

bool MusicPlayState::operator!=(const MusicPlayState &other) const
{
    return !operator ==(other);
}
