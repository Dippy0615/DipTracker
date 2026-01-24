#pragma once
#include "OscillatorType.h"
#include "Constants.h"

class Instrument
{
public:
	Instrument();
	Instrument(const char* instrument_name);
	bool active;
	const char* getName();
	void setName(const char*);
	void setVolumeEnvelopeLength(int length);
	int getVolumeEnvelopeLength();
	void setVolumeEnvelope(int index, int value);
	int* getVolumeEnvelope();
	int getVolumeEnvelopeValue(int index);
	void incrementContinuousTick();
	void resetContinuousTick();
	void setOscillatorType(OscillatorType type);
	OscillatorType getOscillatorType();
private:
	const char* name;
	int volume_envelope[MAX_ENVELOPE_LENGTH];
	int volume_envelope_length;
	int continuous_tick;
	OscillatorType oscillator_type;
	void initializeEnvelope(int envelope[]);

};

