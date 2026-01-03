#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include "Oscillator.h"
#include "OscillatorType.h"
#include "Audio.h"
#include "Pattern.h"
#include "PatternEditor.h"

Pattern patterns[MAX_PATTERNS];
float noise_table[NOISE_TABLE_SIZE];
Channel channels[MAX_CHANNELS];
int sample_counter = 0;
int tick = 0;
int row = 0;
SDL_AudioStream* audio_stream = nullptr;

void populateNoiseTable() {
    for (int i = 0; i < NOISE_TABLE_SIZE; i++) {
        noise_table[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    }
}

float noteToFrequency(int note) {
    int oct = note / 12;
    return ((MIN_FREQUENCY * powf(2, oct)) * (powf(2, (note%12)/12.0f)));
}

void initializeChannels() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        channels[i] = Channel::Channel(Oscillator::Oscillator(OscillatorType::Saw));
    }
}

const char* getNoteName(int note) {
    if (note == NOTE_BLANK) return "---";
    if (note == NOTE_CUT) return "CUT";
    int base_note = note % 12;
    int oct = note / 12;
    char base_string[6];
    switch (base_note) {
        case 0: strcpy_s(base_string,"C-"); break;
        case 1: strcpy_s(base_string, "C#"); break;
        case 2: strcpy_s(base_string, "D-"); break;
        case 3: strcpy_s(base_string, "D#"); break;
        case 4: strcpy_s(base_string, "E-"); break;
        case 5: strcpy_s(base_string, "F-"); break;
        case 6: strcpy_s(base_string, "F#"); break;
        case 7: strcpy_s(base_string, "G-"); break;
        case 8: strcpy_s(base_string, "G#"); break;
        case 9: strcpy_s(base_string, "A-"); break;
        case 10: strcpy_s(base_string, "A#"); break;
        case 11: strcpy_s(base_string, "B-"); break;
    }
    char b[2];
    sprintf_s(b, "%d", oct);
    strcat_s(base_string, b);
    return base_string;
}

const char* getEffectTypeString(int effect) {

    switch(effect){
        case EffectType::NONE: default: return "-";
        case EffectType::VOLUMESLIDE: return "A";
        case EffectType::SPEED: return "F";
    }
}

void resetPlaybackVariablesFull() {
    sample_counter = 0;
    tick = 0;
    row = 0;
    is_editor_jamming = false;
    for (int i = 0; i < MAX_CHANNELS; i++) {
        channels[i].note = NOTE_BLANK;
        channels[i].has_set_volume_this_tick = false;
    }
}