#pragma once

constexpr int SAMPLE_RATE = 48000;
constexpr float FREQUENCY = 261.64f;  // Middle C (C4)
constexpr float MIN_FREQUENCY = FREQUENCY / 8; //Lowest C (C0)
constexpr float SEMITONE_INTERVAL = 1.05946f;
constexpr int NOISE_TABLE_SIZE = 65536;
constexpr int MAX_CHANNELS = 8;
constexpr int MAX_VOLPANEFFECT = 255;
constexpr int DEFAULT_PANNING = 128;