#pragma once

constexpr int SAMPLE_RATE = 48000;
constexpr float FREQUENCY = 261.64f;  // Middle C (C4)
constexpr float MIN_FREQUENCY = FREQUENCY / 8; //Lowest C (C0)
constexpr float SEMITONE_INTERVAL = 1.05946f;
constexpr int NOISE_TABLE_SIZE = 65536;
constexpr int MAX_CHANNELS = 8;
constexpr int MAX_VOLPANEFFECT = 255;
constexpr int DEFAULT_PANNING = 128;
constexpr int NOTE_CUT = 127;
constexpr int NOTE_BLANK = 121;
constexpr int INSTRUMENT_BLANK = 63;
constexpr int VOLUME_BLANK = 127;
constexpr int NOTE_MASK = 0x7F;
constexpr int INSTRUMENT_MASK = 0x1F80;
constexpr int VOLUME_MASK = 0xFE000;
constexpr int EFFECT_MASK = 0xFFF80000;
constexpr int MAX_VOLUME = 64;