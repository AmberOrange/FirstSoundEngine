#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class Pos
{
public:
	Pos(float x, float y, float z, float vx, float vy, float vz);
	~Pos() {};

	XMVECTOR getPosition(int sample);
	void getDistance(const Pos& other, float* buffer, float* buffer2, int deltaSample, int samples);
	//void getDot(const Pos& other, float* buffer, int deltaSample, int samples);
private:
	XMVECTOR initPos;
	XMVECTOR velocity;
	XMVECTOR left;
};