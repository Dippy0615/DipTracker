#pragma once
#include <SDL3/SDL.h>
#include "Constants.h"
#include "Channel.h"

extern float noise_table[NOISE_TABLE_SIZE];
extern Channel channels[MAX_CHANNELS];
extern int sample_counter;
extern int tick;
extern int row;
extern SDL_AudioStream* audio_stream;

enum Note {
	C,
	Csharp,
	D,
	Dsharp,
	E,
	F,
	Fsharp,
	G,
	Gsharp,
	A,
	Asharp,
	B
};

enum EffectType {
	VOLUMESLIDE,
	NONE = 31
};

void populateNoiseTable();
float noteToFrequency(int note);
void initializeChannels();
const char* getNoteName(int note);
const char* getEffectTypeString(int effect);
void resetPlaybackVariablesFull();