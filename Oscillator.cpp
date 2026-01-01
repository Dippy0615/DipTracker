#include <SDL3/SDL.h>
#include <cmath>
#include <cstdlib>
#include "Oscillator.h"
#include "OscillatorType.h"
#include "Constants.h"
#include "Audio.h"

Oscillator::Oscillator() {
	type = OscillatorType::Sine;
	phase = 0.0f;
	frequency = 0.0f;
	noise_time = 0.0f;
	volume = 1.0f;
	volL = 1.0f;
	volR = 1.0f;
	target_volume = 1.0f;
}

Oscillator::Oscillator(OscillatorType type) : type(type) {
	phase = 0.0f;
	frequency = 0.0f;
	noise_time = 0.0f;
	volume = 1.0f;
	volL = 1.0f;
	volR = 1.0f;
	target_volume = 1.0f;
}

void Oscillator::SetType(OscillatorType new_type) {
	type = new_type;
}

void Oscillator::Oscillate(float& sampleL, float& sampleR) {
	if (type == OscillatorType::Sine)
	{
		sampleL = sinf(2.0f * SDL_PI_F * phase);
		sampleR = sampleL;
	}
	else if(type == OscillatorType::Square)
	{
		float cycle_position = fmodf(phase, 1.0f);
		sampleL = cycle_position > 0.5f ? 1.0f : -1.0f; //TO DO: reaplce 0.5 with duty cycle var
		sampleR = sampleL;
	}
	else if (type == OscillatorType::Saw) 
	{
		float cycle_position = (phase * 2.0f) - 1.0f;
		sampleL = cycle_position;
		sampleR = cycle_position;
	}
	else if (type == OscillatorType::Tri)
	{
		sampleL = 4.0f * fabsf(phase - 0.5f) - 1.0f;
		sampleR = sampleL;
	}
	else if (type == OscillatorType::Noise)
	{
		sampleL= noise_table[(int)noise_time];
		sampleR = sampleL;
		noise_time += (51.0f/120) * 0.25f; //TO DO: replace first float w/ note var
		if (noise_time >= NOISE_TABLE_SIZE) noise_time -= NOISE_TABLE_SIZE;
	}
	sampleL *= volume;
	sampleR *= volume;
	sampleL *= volL;
	sampleR *= volR; 
	phase += frequency / SAMPLE_RATE;
	if (phase >= 1.0f) phase -= 1.0;
}

void Oscillator::SetFrequency(float freq) {
	frequency = freq;
}

float Oscillator::GetFrequency() {
	return frequency;
}

void Oscillator::SetOscillatorType(OscillatorType freq) {
	type = freq;
}

OscillatorType Oscillator::GetOscillatorType() {
	return type;
}

void Oscillator::SetVolume(float vol) {
	volume = vol;
}

float Oscillator::GetVolume() {
	return volume;
}

void Oscillator::SetStereo(float left, float right) {
	volL = left;
	volR = right;
}
float Oscillator::GetLeftVolume() {
	return volL;
}
float Oscillator::GetRightVolume() {
	return volR;
}

void Oscillator::SetTargetVolume(float vol) {
	target_volume = vol;
}
float Oscillator::GetTargetVolume() {
	return target_volume;
}