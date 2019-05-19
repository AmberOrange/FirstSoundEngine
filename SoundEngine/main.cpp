//#include <soundio/soundio.h>
#include <iostream>
#include "SoundDevice.hpp"
#include "MasterMixer.hpp"
#include "Osc.hpp"
#include "Spatial.hpp"
#include "Stream.hpp"
#include "Delay.hpp"
//#include "Position.hpp"
#include "Pos.hpp"
#include <Windows.h>
#include <chrono>

//static void write_sample_s16ne(char* ptr, double sample) {
//	int16_t* buf = (int16_t*)ptr;
//	double range = (double)INT16_MAX - (double)INT16_MIN;
//	double val = sample * range / 2.0;
//	*buf = val;
//}
//static void write_sample_s32ne(char* ptr, double sample) {
//	int32_t* buf = (int32_t*)ptr;
//	double range = (double)INT32_MAX - (double)INT32_MIN;
//	double val = sample * range / 2.0;
//	*buf = val;
//}
//static void write_sample_float32ne(char* ptr, double sample) {
//	float* buf = (float*)ptr;
//	*buf = sample;
//}
//static void write_sample_float64ne(char* ptr, double sample) {
//	double* buf = (double*)ptr;
//	*buf = sample;
//}
//static void (*write_sample)(char* ptr, double sample);
//
//static void write_callback(struct SoundIoOutStream* outstream, int frame_count_min, int frame_count_max) {
//
//}
//
//static void underflow_callback(struct SoundIoOutStream* outstream) {
//	static int count = 0;
//	fprintf(stderr, "underflow %d\n", count++);
//}


int main()
{
	//struct SoundIo* soundio = soundio_create();
	//if (!soundio) {
	//	fprintf(stderr, "out of memory\n");
	//	return 1;
	//}

	//int err = soundio_connect(soundio);
	//if (err) {
	//	fprintf(stderr, "Unable to connect to backend: %s\n", soundio_strerror(err));
	//	return 1;
	//}

	//fprintf(stderr, "Backend: %s\n", soundio_backend_name(soundio->current_backend));

	//soundio_flush_events(soundio);
	//int selected_device_index = soundio_default_output_device_index(soundio);
	//
	//if (selected_device_index < 0) {
	//	fprintf(stderr, "Output device not found\n");
	//	return 1;
	//}
	//struct SoundIoDevice* device = soundio_get_output_device(soundio, selected_device_index);
	//if (!device) {
	//	fprintf(stderr, "out of memory\n");
	//	return 1;
	//}
	//fprintf(stderr, "Output device: %s\n", device->name);
	//if (device->probe_error) {
	//	fprintf(stderr, "Cannot probe device: %s\n", soundio_strerror(device->probe_error));
	//	return 1;
	//}

	//struct SoundIoOutStream* outstream = soundio_outstream_create(device);
	//outstream->write_callback = write_callback;
	//outstream->underflow_callback = underflow_callback;
	//outstream->name = NULL;
	//outstream->software_latency = 0.0;
	//outstream->sample_rate = 0;
	//if (soundio_device_supports_format(device, SoundIoFormatFloat32NE)) {
	//	outstream->format = SoundIoFormatFloat32NE;
	//	write_sample = write_sample_float32ne;
	//}
	//else if (soundio_device_supports_format(device, SoundIoFormatFloat64NE)) {
	//	outstream->format = SoundIoFormatFloat64NE;
	//	write_sample = write_sample_float64ne;
	//}
	//else if (soundio_device_supports_format(device, SoundIoFormatS32NE)) {
	//	outstream->format = SoundIoFormatS32NE;
	//	write_sample = write_sample_s32ne;
	//}
	//else if (soundio_device_supports_format(device, SoundIoFormatS16NE)) {
	//	outstream->format = SoundIoFormatS16NE;
	//	write_sample = write_sample_s16ne;
	//}
	//else {
	//	fprintf(stderr, "No suitable device format available.\n");
	//	return 1;
	//}
	//if ((err = soundio_outstream_open(outstream))) {
	//	fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
	//	return 1;
	//}

	//std::cout << outstream->sample_rate << std::endl;

	//soundio_outstream_destroy(outstream);
	//soundio_device_unref(device);
	//soundio_destroy(soundio);
	SoundDevice SD;
	MasterMixer mm;
	SD.init(&mm);
	mm.Init(SD);

	//Position observer({ 0.0, 0.0, 0.0 });
	//Position source({ 120.0, 20.0, 0.0 });
	Pos observer(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Pos source(90.0f, 30.0f, 0.0f, -0.001f, 0.0f, 0.0f);
	Pos source2(-90.0f, 3.0f, 0.0f, 0.0001f, 0.0f, 0.0f);
	mm.AddSoundMod(new Osc(SD, 440.0, 0, 0.2,nullptr,
		new Spatial(&source, &observer)));
	mm.AddSoundMod(new Delay(SD,
		new Stream(SD, "forestbirds.wav",0,0.2), 0.3,0.5));
	//mm.AddSoundMod(new Osc(SD, 435.0, 0, 1.0));
	SD.RefillBuffer();
	SD.Start();
	//auto start = std::chrono::steady_clock::now();
	//auto end = std::chrono::steady_clock::now();
	while (true)
	{
		//start = std::chrono::steady_clock::now();
		SD.RefillBuffer();
		//end = std::chrono::steady_clock::now();
		//std::cout << "Elapsed time in microseconds : "
		//	<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
		//	<< " ms" <<std::endl;
		Sleep(10);
	}
	std::cout << "Hello world!\n";
	std::cin.ignore();
	return 0;
}