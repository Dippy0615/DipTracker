#include "OscillatorType.h"
#include "Instrument.h"

Instrument::Instrument() {
	name = "";
	initializeEnvelope(volume_envelope);
	active = false;
	volume_envelope_length = 0;
	continuous_tick = 0;
	oscillator_type = OscillatorType::Sine;
}

Instrument::Instrument(const char* instrument_name) : name(instrument_name) {
	initializeEnvelope(volume_envelope);
	active = false;
	volume_envelope_length = 0;
	continuous_tick = 0;
	oscillator_type = OscillatorType::Sine;
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

int Instrument::getVolumeEnvelopeValue(int index) {
	return volume_envelope[index];
}

void Instrument::initializeEnvelope(int envelope[]) {
	for (int i = 0; i < MAX_ENVELOPE_LENGTH; i++) {
		envelope[i] = 0;
	}
}

void Instrument::incrementContinuousTick() {
	continuous_tick++;
}

void Instrument::resetContinuousTick() {
	continuous_tick = 0;
}

void Instrument::setOscillatorType(OscillatorType type) {
	oscillator_type = type;
}

OscillatorType Instrument::getOscillatorType() {
	return oscillator_type;
}