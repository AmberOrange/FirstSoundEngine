#pragma once
#include "SoundMod.hpp"

class Delay : public SoundMod
{
public:
	Delay(const SoundDevice& sd,
		SoundMod* source,
		const double time,
		const double decay);
	~Delay() {};

	void output(
		const SoundDevice& sd,
		double** buffer,
		const int frames,
		const int deltaFrame);
private:
	int sampledelay;
	int delaypointer;
	SoundMod* source;
	double** delaybuffer;
	double decay;
};