#include "Vector2d.h"

const Vector2d Vector2d::ZERO = Vector2d(0, 0);
const Vector2d Vector2d::ONE = Vector2d(1, 1);
const Vector2d Vector2d::UP = Vector2d(0, 1);
const Vector2d Vector2d::DOWN = Vector2d(0, -1);
const Vector2d Vector2d::RIGHT = Vector2d(1, 0);
const Vector2d Vector2d::LEFT = Vector2d(-1, 0);

double Vector2d::Magnitude() const
{
	return sqrtf(x * x + y * y);
}

double Vector2d::Magnitude(const Vector2d& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

Vector2d Vector2d::Normalize() const
{
	double magnitude = this->Magnitude();
	if (magnitude == 0)
		return Vector2d::ZERO;
	return Vector2d(this->x / magnitude, this->y / magnitude);
}

Vector2d Vector2d::Normalize(const Vector2d& vec)
{
	double magnitude = vec.Magnitude();
	if (magnitude == 0)
		return Vector2d::ZERO;
	return Vector2d(vec.x / magnitude, vec.y / magnitude);
}

double Vector2d::Dot(const Vector2d& vec2)
{
	return this->x * vec2.x + this->y + vec2.y;
}

double Vector2d::Dot(const Vector2d& vec1, const Vector2d& vec2)
{
	return vec1.x * vec2.x + vec1.y + vec2.y;
}

Vector2d::Vector2d(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector2d::Vector2d()
{
}

void Vector2d::Clear()
{
	this->x = 0;
	this->y = 0;
}
