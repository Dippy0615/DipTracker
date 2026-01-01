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
	bool has_set_volume_this_tick;
	void PlayOscillator(float& sampleL, float& sampleR);
};

