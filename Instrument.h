#pragma once
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
private:
	const char* name;
	int volume_envelope[MAX_ENVELOPE_LENGTH];
	int volume_envelope_length;
	void initializeEnvelope(int envelope[]);

};

