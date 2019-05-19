#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class Position
{
public:
	struct Vec3
	{
		double x, y, z;
		double vx, vy, vz;
		double calcDistance(const Vec3 other);
		double calcRadians(const Vec3 other);
		double calcDot(const Vec3 other);
	};

	Position(const Vec3 position);
	~Position() {};

	Vec3 getPosition() const;
	void setPosition(const Vec3& position);
private:
	Vec3 pos;
};