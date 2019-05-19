#pragma once
#include "SoundMod.hpp"
#include <sndfile.h>

class Stream : public SoundMod
{
public:
	Stream(const SoundDevice& sd,
		const char* file,
		const int channels = 0,
		const double amplitude = 1.0,
		SoundMod* spatMod = nullptr);
	~Stream() {};

	void output(
		const SoundDevice& sd,
		double** buffer,
		const int frames,
		const int deltaFrame);
private:
	double amplitude;
	int channels;
	int sf;
	SoundMod* spatMod;
	double** spatModBuffer;
	SNDFILE* infile;
	SF_INFO info;
};