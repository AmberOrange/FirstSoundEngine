#include "Osc.hpp"
#include "Stream.hpp"
#include "Spatial.hpp"
#include "Delay.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

Osc::Osc(const SoundDevice& sd, const double frequency, const int channels, const double amplitude, SoundMod* freqMod, SoundMod* spatMod)
{
	this->lastPeriod = 0.0;
	this->frequency = frequency;
	this->amplitude = amplitude;
	this->sf = sd.getSampleRate();
	if (channels == 0)
		this->channels = sd.getChannelCount();
	else
		this->channels = channels;

	if (freqMod != nullptr)
	{
		this->freqMod = freqMod;
		this->freqModBuffer = new double*;
		this->freqModBuffer[0] = new double[sd.getBufferSize()];
	}
	else
	{
		this->freqMod = nullptr;
	}

	if (spatMod != nullptr)
	{
		this->spatMod = spatMod;
		this->spatModBuffer = new double*[2];
		this->spatModBuffer[0] = new double[sd.getBufferSize()];
		this->spatModBuffer[1] = new double[sd.getBufferSize()];
	}
	else
	{
		this->spatMod = nullptr;
	}
}

void Osc::output(const SoundDevice& sd, double** buffer, const int frames, const int deltaFrame)
{
	double distance = 1.0;
	if (this->freqMod != nullptr)
		this->freqMod->output(sd, this->freqModBuffer, frames, deltaFrame);
	if (this->spatMod != nullptr)
		this->spatMod->output(sd, this->spatModBuffer, frames, deltaFrame);

	for (int i = 0; i < frames; i++)
	{
		if(this->freqMod != nullptr)
			this->lastPeriod += (this->frequency + this->freqModBuffer[0][i]) / this->sf;
		else
			this->lastPeriod += this->frequency / this->sf;
		
		if (this->spatMod != nullptr)
			distance = this->spatModBuffer[0][i];

		this->lastPeriod = fmod(this->lastPeriod, 1.0);
		for (int j = 0; j < this->channels; j++)
		{
			double pan = 1.0;
			if (this->spatMod != nullptr)
				pan = j == 0 ? pow(sin(this->spatModBuffer[1][i]), 2.0) : pow(cos(this->spatModBuffer[1][i]), 2.0);
			buffer[j][i] = sin(this->lastPeriod * M_PI * 2.0) * this->amplitude * distance * pan;
		}
	}
}

Stream::Stream(const SoundDevice& sd, const char* file, const int channels, const double amplitude, SoundMod* spatMod)
{
	this->amplitude = amplitude;
	this->sf = sd.getSampleRate();
	if (channels == 0)
		this->channels = sd.getChannelCount();
	else
		this->channels = channels;

	if (spatMod != nullptr)
	{
		this->spatMod = spatMod;
		this->spatModBuffer = new double* [2];
		this->spatModBuffer[0] = new double[sd.getBufferSize()];
		this->spatModBuffer[1] = new double[sd.getBufferSize()];
	}
	else
	{
		this->spatMod = nullptr;
	}

	if ((infile = sf_open(file, SFM_READ, &this->info)) == NULL)
	{
		std::cout << "Couldn't load file!\n";
	}
}

void Stream::output(const SoundDevice& sd, double** buffer, const int frames, const int deltaFrame)
{
	double distance = 1.0;
	if (this->spatMod != nullptr)
		this->spatMod->output(sd, this->spatModBuffer, frames, deltaFrame);

	int readcount = 0;
	int totalcount = 0;
	double* buf = new double[frames*channels];
	while (totalcount < frames)
	{
		readcount = sf_readf_double(infile, &buf[totalcount*channels], frames - totalcount);
		if (readcount < frames - totalcount)
			sf_seek(infile, 0, 0);
		totalcount += readcount;
	}

	for (int i = 0; i < frames; i++)
	{
		//if (this->freqMod != nullptr)
		//	this->lastPeriod += (this->frequency + this->freqModBuffer[0][i]) / this->sf;
		//else
		//	this->lastPeriod += this->frequency / this->sf;

		if (this->spatMod != nullptr)
			distance = this->spatModBuffer[0][i];

		//this->lastPeriod = fmod(this->lastPeriod, 1.0);
		for (int j = 0; j < this->channels; j++)
		{
			double pan = 1.0;
			if (this->spatMod != nullptr)
				pan = j == 0 ? pow(sin(this->spatModBuffer[1][i]), 2.0) : pow(cos(this->spatModBuffer[1][i]), 2.0);
			buffer[j][i] = buf[i*channels+j] * this->amplitude * distance * pan;
		}
	}
	delete[] buf;
}


Spatial::Spatial(Pos* source, Pos* observer)
{
	this->source = source;
	this->observer = observer;
}

void Spatial::output(const SoundDevice& sd, double** buffer, const int frames, const int deltaFrame)
{
	//double distance = fmax(0.1,fmin(100.0,
	//	this->observer->getPosition().calcDistance(this->source->getPosition()))/100.0);
	////distance = 1 / pow(distance, 2.0);
	//distance = -log10(distance);
	//double panRatio = this->observer->getPosition().calcDot(this->source->getPosition());
	//panRatio = (panRatio + 1.0) / 2.0;
	//std::cout << panRatio << std::endl;
	float* fbuffer = new float[frames];
	float* fbuffer2 = new float[frames];
	this->observer->getDistance(*this->source, fbuffer, fbuffer2, deltaFrame, frames);
	//std::cout << fbuffer[0] << std::endl;
	for (int i = 0; i < frames; i++)
	{
		buffer[0][i] = pow(-log10(fmax(0.1, fmin(100.0, (double)fbuffer[i]))/100.0),2.0);
		buffer[1][i] = ((double)fbuffer2[i] + 1.0) * M_PI_4;
	}
	delete[] fbuffer;
	delete[] fbuffer2;
}

Delay::Delay(const SoundDevice& sd, SoundMod* source, const double time, const double decay)
{
	this->sampledelay = (int)(sd.getSampleRate() * time);
	this->delaybuffer = new double*[2];
	this->delaybuffer[0] = new double[this->sampledelay];
	this->delaybuffer[1] = new double[this->sampledelay];
	memset(this->delaybuffer[0], 0, sizeof(double)*this->sampledelay);
	memset(this->delaybuffer[1], 0, sizeof(double)*this->sampledelay);
	this->source = source;
	this->delaypointer = 0;
	this->decay = decay;
}

void Delay::output(const SoundDevice& sd, double** buffer, const int frames, const int deltaFrame)
{
	this->source->output(sd, buffer, frames, deltaFrame);
	for (int j = 0; j < frames; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			buffer[i][j] += this->delaybuffer[i][this->delaypointer] * this->decay;
			this->delaybuffer[i][this->delaypointer] = buffer[i][j];

		}
		this->delaypointer++;
		this->delaypointer %= sampledelay;
	}


}
