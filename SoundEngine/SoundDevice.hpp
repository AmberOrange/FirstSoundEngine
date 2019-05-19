#pragma once

class MasterMixer;
class SoundDevice
{
public:
	SoundDevice();
	bool init(MasterMixer* mm);
	void RefillBuffer();
	void Start();

	// Get
	int getSampleRate() const;
	int getChannelCount() const;
	int getBufferSize() const;
private:
	int initSoundio();
	int initDevice();
	int initOutstream();
	int initBuffer();
	
	void RequestRefill();

	static void write_callback(struct SoundIoOutStream* outstream, int frame_count_min, int frame_count_max);
	static void underflow_callback(struct SoundIoOutStream* outstream);

	void (*WriteSample)(char* ptr, double sample);

	struct SoundIo* mSoundio;
	struct SoundIoDevice* mDevice;
	struct SoundIoOutStream* mOutstream;

	int mBufferSize;
	double** mBuffer;
	int mBufferCounter;
	int deltaFrame;

	MasterMixer* pMasterMixer;

	volatile bool vRefill;
};