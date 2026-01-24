#include "Constants.h"
#include "Oscillator.h"
#include "Channel.h"
#include "Audio.h"

Channel::Channel() {
	my_oscillator = Oscillator::Oscillator(OscillatorType::Saw);
	volume = MAX_VOLPANEFFECT;
	panning = DEFAULT_PANNING;
	note = -1;
	is_on = true;
	has_set_volume_this_row = false;
	has_set_continuous_tick_this_row = false;
	has_set_envelope_this_tick = false;
	continuous_tick = 0;
}

Channel::Channel(Oscillator osc) : my_oscillator(osc) {
	volume = MAX_VOLPANEFFECT;
	panning = DEFAULT_PANNING;
	note = -1;
	is_on = true;
	has_set_volume_this_row = false;
	has_set_continuous_tick_this_row = false;
	has_set_envelope_this_tick = false;
	continuous_tick = 0;
}

void Channel::PlayOscillator(float& sampleL, float& sampleR) {
	if (!is_on)
	{
		sampleL = 0.0f;
		sampleR = 0.0f;
		return;
	}

	my_oscillator.SetFrequency(noteToFrequency(note));
	//my_oscillator.SetVolume((float)(volume/MAX_VOLPANEFFECT));
	my_oscillator.Oscillate(sampleL, sampleR);
}