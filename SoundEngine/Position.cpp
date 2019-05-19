#include "Position.hpp"
#include <math.h>

Position::Position(const Vec3 position)
{
	this->pos = position;
}

Position::Vec3 Position::getPosition() const
{
	return this->pos;
}

void Position::setPosition(const Vec3& position)
{
	this->pos = position;
}

double Position::Vec3::calcDistance(const Vec3 other)
{
	return sqrt(pow(other.x - x, 2.0) + pow(other.y - y, 2.0) + pow(other.z - z, 2.0));
}

double Position::Vec3::calcRadians(const Vec3 other)
{
	return atan2(other.y - y, other.x - x);
}

double Position::Vec3::calcDot(const Vec3 other)
{
	//const Vec3 left = { 1.0, 0.0, 0.0 };
	double length = calcDistance(other);
	return (other.x - x) / length;
}