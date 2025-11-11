#include "Constants.h"
#include "Oscillator.h"
#include "Channel.h"
#include "Audio.h"

Channel::Channel() {
	my_oscillator = Oscillator::Oscillator(OscillatorType::Saw);
	volume = MAX_VOLPANEFFECT;
	panning = DEFAULT_PANNING;
	note = 127;
	is_on = true;
}

Channel::Channel(Oscillator osc) : my_oscillator(osc) {
	volume = MAX_VOLPANEFFECT;
	panning = DEFAULT_PANNING;
	note = 127;
	is_on = true;
}

void Channel::PlayOscillator(float& sampleL, float& sampleR) {
	if (!is_on)
	{
		sampleL = 0.0f;
		sampleR = 0.0f;
		return;
	}

	my_oscillator.SetFrequency(noteToFrequency(note));
	my_oscillator.SetVolume((float)(volume/MAX_VOLPANEFFECT));
	my_oscillator.Oscillate(sampleL, sampleR);
}