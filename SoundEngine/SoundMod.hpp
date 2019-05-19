#pragma once
#include "SoundDevice.hpp"

class SoundMod
{
public:
	SoundMod() {};
	virtual ~SoundMod() {};

	virtual void output(
		const SoundDevice& sd,
		double** buffer,
		const int frames,
		const int deltaFrame) = 0;
};