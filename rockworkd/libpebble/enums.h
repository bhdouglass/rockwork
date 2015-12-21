#ifndef ENUMS_H
#define ENUMS_H

enum HardwareRevision {
    HardwareRevisionUNKNOWN = 0,
    HardwareRevisionTINTIN_EV1 = 1,
    HardwareRevisionTINTIN_EV2 = 2,
    HardwareRevisionTINTIN_EV2_3 = 3,
    HardwareRevisionTINTIN_EV2_4 = 4,
    HardwareRevisionTINTIN_V1_5 = 5,
    HardwareRevisionBIANCA = 6,
    HardwareRevisionSNOWY_EVT2 = 7,
    HardwareRevisionSNOWY_DVT = 8,
    HardwareRevisionSPALDING_EVT = 9,
    HardwareRevisionBOBBY_SMILES = 10,
    HardwareRevisionSPALDING = 11,

    HardwareRevisionTINTIN_BB = 0xFF,
    HardwareRevisionTINTIN_BB2 = 0xFE,
    HardwareRevisionSNOWY_BB = 0xFD,
    HardwareRevisionSNOWY_BB2 = 0xFC,
    HardwareRevisionSPALDING_BB2 = 0xFB
};

enum HardwarePlatform {
    HardwarePlatformUnknown = 0,
    HardwarePlatformAplite,
    HardwarePlatformBasalt,
    HardwarePlatformChalk
};

enum MusicControlButton {
    MusicControlPlayPause,
    MusicControlSkipBack,
    MusicControlSkipNext,
    MusicControlVolumeUp,
    MusicControlVolumeDown
};

enum CallStatus {
    CallStatusIncoming,
    CallStatusOutGoing
};

#endif // ENUMS_H

