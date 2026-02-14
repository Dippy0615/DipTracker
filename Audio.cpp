#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "Oscillator.h"
#include "OscillatorType.h"
#include "Audio.h"
#include "Pattern.h"
#include "PatternEditor.h"
#include "Instrument.h"

Pattern patterns[MAX_PATTERNS];
float noise_table[NOISE_TABLE_SIZE];
Channel channels[MAX_CHANNELS];
Instrument instruments[MAX_INSTRUMENTS];
int sample_counter = 0;
int tick = 0;
int row = 0;
float samples_per_tick = SAMPLE_RATE / 60.0f;
int ticks_per_row = 6;
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

void initializeInstruments() {
    for (int i = 0; i < MAX_INSTRUMENTS; i++) {
        char name[15];
        instruments[i] = Instrument::Instrument("Instrument");
    }
    instruments[0].active = true;
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
        channels[i].my_oscillator.SetVolume(0.0f);
        channels[i].note = NOTE_BLANK;
        channels[i].has_set_volume_this_row = false;
        channels[i].has_set_continuous_tick_this_row = false;
        channels[i].has_set_envelope_this_tick = false;
        channels[i].continuous_tick = 0;
    }
}

void processSamples(float* buffer, int samples_to_go) {
    for (int i = 0; i < samples_to_go; i++) {
        float sampleL = 0;
        float sampleR = 0;

        if (editor_mode == PatternEditorMode::PLAY) {
            for (int i = 0; i < MAX_CHANNELS; i++) {
                float left = 0.0f;
                float right = 0.0f;

                int note = current_pattern->getCellNote(row, i);
                int volume = current_pattern->getCellVolume(row, i);
                int instrument = current_pattern->getCellInstrument(row, i);
                int effect_type = current_pattern->getCellEffectType(row, i);
                int effect_one = current_pattern->getCellEffectOne(row, i);
                int effect_two = current_pattern->getCellEffectTwo(row, i);

                Oscillator* osc = &channels[i].my_oscillator;
                Instrument* ins = &instruments[instrument];

                //Set the volume if not set already
                if (volume != VOLUME_BLANK && volume > -1 && !channels[i].has_set_volume_this_row) {
                    osc->SetTargetVolume((float)volume / MAX_VOLUME);
                    channels[i].volume = volume;
                    channels[i].has_set_volume_this_row = true;
                }

                //Instrument
                if (instrument != INSTRUMENT_BLANK) {
                    channels[i].current_instrument = instrument;
                    osc->SetOscillatorType(ins->getOscillatorType());
                }

                //Instrument's volume envelope
                Instrument* channel_ins = &instruments[channels[i].current_instrument];
                int len = channel_ins->getVolumeEnvelopeLength();
                if (len > 0 && channels[i].continuous_tick < len && !channels[i].has_set_envelope_this_tick) {
                    float percentage = (float)channel_ins->getVolumeEnvelopeValue(channels[i].continuous_tick) / (float)MAX_VOLUME;
                    float channel_percentage = (float)(channels[i].volume) / (float)MAX_VOLUME;
                    osc->SetTargetVolume(percentage * channel_percentage);
                    channels[i].has_set_envelope_this_tick = true;
                }

                //Implement effects
                if (effect_type == EffectType::VOLUMESLIDE) {
                    //Prioritizes fade out
                    if (effect_two > 0) { //Fade out
                        osc->SetTargetVolume(osc->GetTargetVolume() - ((float)effect_two / MAX_EFFECT_VALUE) * 0.00025f);
                        if (osc->GetTargetVolume() < 0) osc->SetTargetVolume(0);
                    }
                    else if (effect_one > 0) { //Fade in
                        osc->SetTargetVolume(osc->GetTargetVolume() + ((float)effect_one / MAX_EFFECT_VALUE) * 0.00025f);
                        if (osc->GetTargetVolume() > 1) osc->SetTargetVolume(1);
                    }
                }
                if (effect_type == EffectType::SPEED) {
                    int speed = (effect_two + (effect_one * 16));
                    if (speed > 0) ticks_per_row = speed;
                }

                float current_volume = osc->GetVolume();
                if (current_volume != osc->GetTargetVolume()) {
                    //Volume smoothing
                    float tvol = osc->GetTargetVolume();
                    float step = (tvol - current_volume) * 0.009f;
                    bool negative = signbit(step);
                    current_volume += step;
                    if ((negative && current_volume < tvol) || (!negative && current_volume > tvol)) current_volume = tvol;
                    osc->SetVolume(current_volume);
                }

                if (note == NOTE_BLANK && channels[i].note == NOTE_BLANK) channels[i].note = NOTE_CUT;
                if (note != NOTE_BLANK) {
                    channels[i].note = note;
                    if(!channels[i].has_set_continuous_tick_this_row){
                        channels[i].continuous_tick = 0;
                        channels[i].has_set_continuous_tick_this_row = true;
                    }
                }
                if (channels[i].note != NOTE_CUT) channels[i].PlayOscillator(left, right);

                sampleL += left;
                sampleR += right;
            }
        }
        else if (is_editor_jamming) {
            float left = 0.0f;
            float right = 0.0f;
            channels[preview_channel].my_oscillator.SetVolume(1.0f);
            channels[preview_channel].note = preview_note;
            channels[preview_channel].my_oscillator.SetOscillatorType(instruments[preview_instrument].getOscillatorType());
            channels[preview_channel].PlayOscillator(left, right);
            sampleL += left;
            sampleR += right;
        }
        sampleL *= 0.5f;
        sampleR *= 0.5f;

        sample_counter++;

        if (editor_mode == PatternEditorMode::PLAY) {

            if (sample_counter >= samples_per_tick) {
                sample_counter -= samples_per_tick;
                tick++;
                for (int i = 0; i < MAX_CHANNELS; i++) {
                    channels[i].continuous_tick++;
                    channels[i].has_set_envelope_this_tick = false;
                }

                if (tick >= ticks_per_row) {
                    tick = 0;
                    row++;
                    //Reset flags
                    for (int i = 0; i < MAX_CHANNELS; i++) {
                        channels[i].has_set_volume_this_row = false;
                        channels[i].has_set_continuous_tick_this_row = false;
                    }

                    //if(row>pattern.row_count/2) first_row_to_render++;
                    if (row == current_pattern->row_count) {
                        row = 0;
                        first_row_to_render = 0;
                        if (++current_pattern_index == patterns_active) {
                            current_pattern_index = 0;
                        }
                        current_pattern = &patterns[current_pattern_index];
                    }
                }

            }
        }

        if (--preview_time < 0) {
            preview_time = 0;
            is_editor_jamming = false;
        }
        buffer[i * 2 + 0] = sampleL; // Left
        buffer[i * 2 + 1] = sampleR; // Right
    }
}