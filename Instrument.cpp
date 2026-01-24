#include "Instrument.h"

Instrument::Instrument() {
	name = "";
	initializeEnvelope(volume_envelope);
	active = false;
	volume_envelope_length = 0;
}

Instrument::Instrument(const char* instrument_name) : name(instrument_name) {
	initializeEnvelope(volume_envelope);
	active = false;
	volume_envelope_length = 0;
}

const char* Instrument::getName() {
	return name;
}

void Instrument::setName(const char* new_name) {
	name = new_name;
}

void Instrument::setVolumeEnvelopeLength(int length) {
	volume_envelope_length = length;
}
int Instrument::getVolumeEnvelopeLength() {
	return volume_envelope_length;
}

void Instrument::setVolumeEnvelope(int index, int value) {
	volume_envelope[index] = value;
}

int* Instrument::getVolumeEnvelope() {
	return volume_envelope;
}

void Instrument::initializeEnvelope(int envelope[]) {
	for (int i = 0; i < MAX_ENVELOPE_LENGTH; i++) {
		envelope[i] = 0;
	}
}