#include "Pos.hpp"

Pos::Pos(float x, float y, float z, float vx, float vy, float vz)
{
	XMFLOAT3 temp = XMFLOAT3(x, y, z);
	this->initPos = XMLoadFloat3(&temp);
	temp = XMFLOAT3(vx, vy, vz);
	this->velocity = XMLoadFloat3(&temp);
	temp = XMFLOAT3(1.0f, 0.f, 0.f);
	this->left = XMLoadFloat3(&temp);
}

XMVECTOR Pos::getPosition(int sample)
{
	return this->initPos + this->velocity * sample;
}

void Pos::getDistance(const Pos& other, float* buffer, float* buffer2, int deltaSample, int samples)
{
	for (int i = 0; i < samples; i++)
	{
		int currS = deltaSample - samples + i + 1;
		XMVECTOR vec =
			(this->initPos + this->velocity * currS) -
			(other.initPos + other.velocity * currS);
		XMVECTOR len = XMVector3Length(vec);
		XMStoreFloat(&buffer[i], len);
		vec = XMVector3Dot(vec, this->left) / -len;
		XMStoreFloat(&buffer2[i], vec);
	}
}
