#pragma once
#include "OscillatorType.h"

class Oscillator {
private:
	OscillatorType type;
	float phase;
	float frequency;
	float noise_time;
	float volume;
	float volL;
	float volR;
	float target_volume;
public:
	Oscillator();
	Oscillator(OscillatorType type);
	void Oscillate(float& sampleL, float& sampleR);
	void SetType(OscillatorType new_type);
	void SetFrequency(float freq);
	float GetFrequency();
	void SetOscillatorType(OscillatorType type);
	OscillatorType GetOscillatorType();
	float GetVolume();
	void SetVolume(float vol);
	void SetStereo(float left, float right);
	float GetLeftVolume();
	float GetRightVolume();
	void SetTargetVolume(float vol);
	float GetTargetVolume();
};