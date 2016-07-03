#ifndef MUSICMETADATA_H
#define MUSICMETADATA_H

#include <QString>

class MusicMetaData
{
public:
    MusicMetaData();
    MusicMetaData(const QString &artist, const QString &album, const QString &title, qlonglong length);

    QString artist;
    QString album;
    QString title;
    qlonglong length = 0;
    int trackCount = 0;
    int currentTrack = 0;

    bool operator==(const MusicMetaData &other) const;
    bool operator!=(const MusicMetaData &other) const;
};

class MusicPlayState
{
public:
    enum State {
        StatePaused = 0x00,
        StatePlaying = 0x01,
        StateRewinding = 0x02,
        StateFastforwarding = 0x03,
        StateUnknown = 0x04
    };
    enum Shuffle {
        ShuffleUnknown = 0x00,
        ShuffleOff = 0x01,
        ShuffleOn = 0x02
    };
    enum Repeat {
        RepeatUnknown = 0x00,
        RepeatOff = 0x01,
        RepeatOne = 0x02,
        RepeatAll = 0x02
    };

    MusicPlayState();
    MusicPlayState(const State state, const qint32 trackPosition, const qint32 playRate, const Shuffle shuffle, const Repeat repeat);

    bool operator==(const MusicPlayState &other) const;
    bool operator!=(const MusicPlayState &other) const;

    State state = StatePaused;
    qint32 trackPosition = 0;
    qint32 playRate = 100;
    Shuffle shuffle = ShuffleOff;
    Repeat repeat = RepeatOff;
};
#endif // MUSICMETADATA_H
