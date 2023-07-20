#include "Vectorf.h"

const Vector2d Vector2d::ZERO = Vector2d(0, 0);
const Vector2d Vector2d::ONE = Vector2d(1, 1);
const Vector2d Vector2d::UP = Vector2d(0, 1);
const Vector2d Vector2d::DOWN = Vector2d(0, -1);
const Vector2d Vector2d::RIGHT = Vector2d(1, 0);
const Vector2d Vector2d::LEFT = Vector2d(-1, 0);

const Vector3d Vector3d::ZERO = Vector3d(0, 0, 0);
const Vector3d Vector3d::ONE = Vector3d(1, 1, 1);
const Vector3d Vector3d::FORWARD = Vector3d(0, 0, 1);
const Vector3d Vector3d::BACK = Vector3d(0, 0, -1);
const Vector3d Vector3d::LEFT = Vector3d(-1, 0, 0);
const Vector3d Vector3d::RIGHT = Vector3d(1, 0, 0);
const Vector3d Vector3d::UP = Vector3d(0, 1, 0);
const Vector3d Vector3d::DOWN = Vector3d(0, -1, 0);

float Vector2d::Magnitude() const
{
	return sqrtf(x * x + y * y);
}

float Vector2d::Magnitude(const Vector2d& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

Vector2d Vector2d::Normalize() const
{
	float magnitude = this->Magnitude();
	if (magnitude == 0)
		return Vector2d::ZERO;
	return Vector2d(this->x / magnitude, this->y / magnitude);
}

Vector2d Vector2d::Normalize(const Vector2d& vec)
{
	float magnitude = vec.Magnitude();
	if (magnitude == 0)
		return Vector2d::ZERO;
	return Vector2d(vec.x / magnitude, vec.y / magnitude);
}

float Vector2d::Dot(const Vector2d& vec2)
{
	return this->x * vec2.x + this->y + vec2.y;
}

float Vector2d::Dot(const Vector2d& vec1, const Vector2d& vec2)
{
	return vec1.x * vec2.x + vec1.y + vec2.y;
}

Vector2d::Vector2d(float x, float y)
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

/////////////////////////////////////VECTOR3D/////////////////////////////////////////////////

Vector3d::Vector3d(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3d::Vector3d()
{
	this->Clear();
}

void Vector3d::Clear()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

float Vector3d::Magnitude() const
{
	return sqrt(x * x + y * y + z * z);
}

float Vector3d::Magnitude(const Vector3d& vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vector3d Vector3d::Normalize() const
{
	float norm = this->Magnitude();
	if(norm != 0) return Vector3d(x / norm, y / norm, z / norm);
	else return Vector3d(0, 0, 0);
}

Vector3d Vector3d::Normalize(const Vector3d& vec)
{
	float norm = vec.Magnitude();
	if (norm != 0) return Vector3d(vec.x / norm, vec.y / norm, vec.z / norm);
	else return Vector3d(0, 0, 0);
}

float Vector3d::Dot(const Vector3d& vec2)
{
	return this->x * vec2.x + this->y * vec2.y + this->z * vec2.z;
}

float Vector3d::Dot(const Vector3d& vec1, const Vector3d& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}
