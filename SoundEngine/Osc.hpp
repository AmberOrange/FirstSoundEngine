#pragma once
#include "SoundMod.hpp"

class Osc : public SoundMod
{
public:
	Osc(const SoundDevice& sd,
		const double frequency,
		const int channels = 0,
		const double amplitude = 1.0,
		SoundMod* freqMod = nullptr,
		SoundMod* spatMod = nullptr);
	~Osc() {};

	void output(
		const SoundDevice& sd,
		double** buffer,
		const int frames,
		const int deltaFrame);
private:
	double lastPeriod;
	double frequency;
	double amplitude;
	int channels;
	int sf;
	SoundMod* freqMod;
	double** freqModBuffer;
	SoundMod* spatMod;
	double** spatModBuffer;
};