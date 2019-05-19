#pragma once
#include "SoundMod.hpp"
//#include "Position.hpp"
#include "Pos.hpp"

class Spatial : public SoundMod
{
public:
	Spatial(Pos* source, Pos* observer);
	~Spatial() {};

	void output(
		const SoundDevice& sd,
		double** buffer,
		const int frames,
		const int deltaFrame);
private:
	Pos* source;
	Pos* observer;
};