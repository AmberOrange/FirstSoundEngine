#include "MasterMixer.hpp"
#include "SoundDevice.hpp"
#include "SoundMod.hpp"
#include <cstring>

void MasterMixer::Init(const SoundDevice& sd)
{
	this->tempBuffer = new double* [sd.getChannelCount()];
	for (int i = 0; i < sd.getChannelCount(); i++)
		this->tempBuffer[i] = new double[sd.getBufferSize()];
}

void MasterMixer::AddSoundMod(SoundMod* sm)
{
	this->mSoundModList.push_back(sm);
}

int MasterMixer::generateSamples(const SoundDevice& sd, double** buffer, const int frames, const int deltaFrame)
{
	ZeroBuffer(buffer, sd.getChannelCount(), frames);
	for (int i = 0; i < mSoundModList.size(); i++)
	{
		this->mSoundModList[i]->output(sd, this->tempBuffer, frames, deltaFrame);
		for (int i = 0; i < sd.getChannelCount(); i++)
		{
			for (int j = 0; j < frames; j++)
			{
				buffer[i][j] += this->tempBuffer[i][j];
			}
		}
	}
	return 0;
}

void MasterMixer::ZeroBuffer(double** buffer, int channels, int frames)
{
	for (int i = 0; i < channels; i++)
	{
		memset(buffer[i], 0, sizeof(double) * frames);
	}
}
