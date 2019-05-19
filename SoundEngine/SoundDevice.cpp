#include "SoundDevice.hpp"
#include "MasterMixer.hpp"
#define SOUNDIO_STATIC_LIBRARY
#include <soundio/soundio.h>
#include <iostream>

static void write_sample_s16ne(char* ptr, double sample) {
	int16_t* buf = (int16_t*)ptr;
	double range = (double)INT16_MAX - (double)INT16_MIN;
	double val = sample * range / 2.0;
	*buf = (int16_t)val;
}
static void write_sample_s32ne(char* ptr, double sample) {
	int32_t* buf = (int32_t*)ptr;
	double range = (double)INT32_MAX - (double)INT32_MIN;
	double val = sample * range / 2.0;
	*buf = (int32_t)val;
}
static void write_sample_float32ne(char* ptr, double sample) {
	float* buf = (float*)ptr;
	*buf = (float)sample;
}
static void write_sample_float64ne(char* ptr, double sample) {
	double* buf = (double*)ptr;
	*buf = sample;
}

SoundDevice::SoundDevice()
{
	this->mSoundio = 0;
	this->mDevice = 0;
	this->mOutstream = 0;
	this->WriteSample = 0;
	this->deltaFrame = 0;
}

bool SoundDevice::init(MasterMixer* mm)
{
	this->pMasterMixer = mm;
	if (this->initSoundio()) {
		std::cerr << "Couldn't init Soundio.\n";
		return false;
	}
	else if (this->initDevice()) {
		std::cerr << "Couldn't init Device.\n";
		return false;
	}
	else if (this->initOutstream()) {
		std::cerr << "Couldn't init Output stream.\n";
		return false;
	}
	else if (this->initBuffer()) {
		std::cerr << "Couldn't init Buffer.\n";
		return false;
	}
	return true;
}

void SoundDevice::RefillBuffer()
{
	if (this->vRefill)
	{
		if(mBufferCounter != mBufferSize)
			std::cout << "Buffer Counter: " << this->mBufferCounter << std::endl;
		//std::cout << "*";
		this->mBufferCounter = 0;
		this->pMasterMixer->generateSamples(*this, this->mBuffer, this->mBufferSize, deltaFrame);
		deltaFrame += this->mBufferSize;
		this->vRefill = false;
	}
}

void SoundDevice::Start()
{
	int err;
	if ((err = soundio_outstream_start(this->mOutstream))) {
		fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
	}
}

int SoundDevice::getSampleRate() const
{
	return this->mOutstream->sample_rate;
}

int SoundDevice::getChannelCount() const
{
	return this->mOutstream->layout.channel_count;
}

int SoundDevice::getBufferSize() const
{
	return this->mBufferSize;
}

int SoundDevice::initSoundio()
{
	this->mSoundio = soundio_create();
	if (!this->mSoundio) {
		std::cerr << "out of memory\n";
		return 1;
	}

	int err = soundio_connect(this->mSoundio);
	if (err) {
		std::cerr << "Unable to connect to backend: "
			<< soundio_strerror(err) << std::endl;
		return 1;
	}

	std::cout << "Backend: "
		<< soundio_backend_name(this->mSoundio->current_backend) << std::endl;
	return 0;
}

int SoundDevice::initDevice()
{
	soundio_flush_events(this->mSoundio);
	int selected_device_index = soundio_default_output_device_index(this->mSoundio);

	if (selected_device_index < 0) {
		std::cerr << "Output device not found\n";
		return 1;
	}

	this->mDevice = soundio_get_output_device(this->mSoundio, selected_device_index);
	if (!this->mDevice) {
		std::cerr << "out of memory\n";
		return 1;
	}
	std::cerr << "Output device: "
		<< this->mDevice->name << std::endl;
	if (this->mDevice->probe_error) {
		std::cerr << "Cannot probe device: "
			<< soundio_strerror(this->mDevice->probe_error) << std::endl;
		return 1;
	}

	return 0;
}

int SoundDevice::initOutstream()
{
	int err;
	this->mOutstream = soundio_outstream_create(this->mDevice);
	this->mOutstream->write_callback = write_callback;
	this->mOutstream->underflow_callback = underflow_callback;
	this->mOutstream->name = NULL;
	this->mOutstream->software_latency = 0.0;
	this->mOutstream->sample_rate = 44100;
	this->mOutstream->userdata = this;
	if (soundio_device_supports_format(this->mDevice, SoundIoFormatFloat32NE)) {
		this->mOutstream->format = SoundIoFormatFloat32NE;
		this->WriteSample = write_sample_float32ne;
	}
	else if (soundio_device_supports_format(this->mDevice, SoundIoFormatFloat64NE)) {
		this->mOutstream->format = SoundIoFormatFloat64NE;
		this->WriteSample = write_sample_float64ne;
	}
	else if (soundio_device_supports_format(this->mDevice, SoundIoFormatS32NE)) {
		this->mOutstream->format = SoundIoFormatS32NE;
		this->WriteSample = write_sample_s32ne;
	}
	else if (soundio_device_supports_format(this->mDevice, SoundIoFormatS16NE)) {
		this->mOutstream->format = SoundIoFormatS16NE;
		this->WriteSample = write_sample_s16ne;
	}
	else {
		std::cerr << "No suitable device format available.\n";
		return 1;
	}
	if ((err = soundio_outstream_open(this->mOutstream))) {
		std::cerr << "unable to open device: " << soundio_strerror(err) << std::endl;
		return 1;
	}
	std::cout << this->mOutstream->software_latency << std::endl;
	return 0;
}

int SoundDevice::initBuffer()
{
	int err;
	// Check if buffer can be cleared
	err = soundio_outstream_clear_buffer(this->mOutstream);
	if (err == SoundIoError::SoundIoErrorIncompatibleBackend ||
		err == SoundIoError::SoundIoErrorIncompatibleDevice)
	{
		this->mBufferSize = (int)(this->mOutstream->sample_rate *
			this->mOutstream->software_latency);
	}
	else {
		// If buffer size isn't strict, set a custom one
		this->mBufferSize = 2048;
		// If it's too big (which I don't think it should ever be)
		// shrink it to the maximum allowed size
		if (this->mBufferSize / this->mOutstream->sample_rate >
			this->mOutstream->software_latency)
			this->mBufferSize = (int)(this->mOutstream->sample_rate *
				this->mOutstream->software_latency);
	}

	this->mBuffer = new double*[this->mOutstream->layout.channel_count];
	for (int i = 0; i < this->mOutstream->layout.channel_count; i++)
		this->mBuffer[i] = new double[this->mBufferSize];
	this->mBufferCounter = 0;
	return 0;
}

void SoundDevice::RequestRefill()
{
	this->vRefill = true;
}

void SoundDevice::write_callback(SoundIoOutStream* outstream, int frame_count_min, int frame_count_max)
{
	SoundDevice* sd = (SoundDevice*)outstream->userdata;
	struct SoundIoChannelArea* areas;
	int err;
	int frames_left = sd->getBufferSize(); //frame_count_max
	int bufferCounter = sd->mBufferCounter;
	//std::cout << frame_count_min << std::endl
	//	<< frame_count_max << std::endl;
	for (;;) {
		int frame_count = frames_left;
		if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
			fprintf(stderr, "unrecoverable stream error: %s\n", soundio_strerror(err));
			exit(1);
		}
		if (!frame_count)
			break;
		const struct SoundIoChannelLayout* layout = &outstream->layout;
		for (int frame = 0; frame < frame_count; frame += 1) {
			for (int channel = 0; channel < layout->channel_count; channel += 1) {
				sd->WriteSample(areas[channel].ptr, sd->mBuffer[channel][frame]);
				areas[channel].ptr += areas[channel].step;
			}
			bufferCounter++;
		}

		if ((err = soundio_outstream_end_write(outstream))) {
			if (err == SoundIoErrorUnderflow)
				return;
			fprintf(stderr, "unrecoverable stream error: %s\n", soundio_strerror(err));
			exit(1);
		}
		frames_left -= frame_count;
		if (frames_left <= 0)
			break;
	}
	sd->RequestRefill();
	sd->mBufferCounter = bufferCounter;
	//soundio_outstream_pause(outstream, want_pause);
}

void SoundDevice::underflow_callback(SoundIoOutStream* outstream)
{
	static int count = 0;
	std::cerr << "underflow " << count++ << std::endl;
}
