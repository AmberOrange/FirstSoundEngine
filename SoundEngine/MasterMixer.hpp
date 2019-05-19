
#include <vector>
class SoundMod;
class SoundDevice;
class MasterMixer
{
public:
	MasterMixer() {};
	void Init(const SoundDevice& sd);
	void AddSoundMod(SoundMod* sm);

	int generateSamples(const SoundDevice& sd, double** buffer, const int frames, const int deltaFrame);
private:
	void ZeroBuffer(double** buffer, int channels, int frames);
	std::vector<SoundMod*> mSoundModList;
	double** tempBuffer;
};