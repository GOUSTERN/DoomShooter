#pragma once
#include <cmath>

class Vector2d
{
public:
	double x = 0, y = 0;

	Vector2d(double x, double y);
	Vector2d();
	/// <summary>
	/// sets x and y to zero
	/// </summary>
	void Clear();
	/// <summary>
	/// returns magnitude of this vector
	/// </summary>
	double Magnitude() const;
	/// <summary>
	/// returns magnitude of vector from parameters
	/// </summary>
	static double Magnitude(const Vector2d& vec);
	/// <summary>
	/// returns normalized vector of this vector
	/// </summary>
	Vector2d Normalize() const;
	/// <summary>
	/// returns normalized vector of vector from parameters
	/// </summary>
	static Vector2d Normalize(const Vector2d& vec);
	/// <summary>
	/// returns dot of this vector and vector from parameters
	/// </summary>
	double Dot(const Vector2d& vec2);
	/// <summary>
	/// returns dot of vectors from parameters
	/// </summary>
	static double Dot(const Vector2d& vec1, const Vector2d& vec2);

	inline Vector2d operator+(const Vector2d& other) const { return Vector2d(this->x + other.x, this->y + other.y); }
	inline Vector2d operator-(const Vector2d& other) const { return Vector2d(this->x - other.x, this->y - other.y); }
	inline Vector2d operator*(const Vector2d& other) const { return Vector2d(this->x * other.x, this->y * other.y); }
	inline Vector2d operator*(const double& other) const { return Vector2d(this->x * other, this->y * other); }
	inline Vector2d operator/(const Vector2d& other) const { return Vector2d(this->x / other.x, this->y / other.y); }
	inline Vector2d operator/(const double& other) const { return Vector2d(this->x / other, this->y / other); }

	/// <summary>
	/// 0, 0
	/// </summary>
	static const Vector2d ZERO;
	/// <summary>
	/// 1, 1
	/// </summary>
	static const Vector2d ONE;
	/// <summary>
	/// 0, 1
	/// </summary>
	static const Vector2d UP;
	/// <summary>
	/// 0, -1
	/// </summary>
	static const Vector2d DOWN;
	/// <summary>
	/// 1, 0
	/// </summary>
	static const Vector2d RIGHT;
	/// <summary>
	/// -1, 0
	/// </summary>
	static const Vector2d LEFT;
};