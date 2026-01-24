#pragma once
#include "Oscillator.h"

class Channel
{
public:
	Channel();
	Channel(Oscillator osc);
	Oscillator my_oscillator;
	int volume;
	int panning;
	int note;
	bool is_on;
	bool has_set_volume_this_row;
	bool has_set_continuous_tick_this_row;
	bool has_set_envelope_this_tick;
	int continuous_tick;
	void PlayOscillator(float& sampleL, float& sampleR);
};

