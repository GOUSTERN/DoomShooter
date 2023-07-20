#pragma once
#include <cmath>

class Vector2d
{
public:
	float x = 0, y = 0;

	Vector2d(float x, float y);
	Vector2d();
	/// <summary>
	/// sets x and y to zero
	/// </summary>
	void Clear();
	/// <summary>
	/// returns magnitude of this vector
	/// </summary>
	float Magnitude() const;
	/// <summary>
	/// returns magnitude of vector from parameters
	/// </summary>
	static float Magnitude(const Vector2d& vec);
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
	float Dot(const Vector2d& vec2);
	/// <summary>
	/// returns dot of vectors from parameters
	/// </summary>
	static float Dot(const Vector2d& vec1, const Vector2d& vec2);

	inline Vector2d operator+(const Vector2d& other) const { return Vector2d(this->x + other.x, this->y + other.y); }
	inline Vector2d operator-(const Vector2d& other) const { return Vector2d(this->x - other.x, this->y - other.y); }
	inline Vector2d operator*(const Vector2d& other) const { return Vector2d(this->x * other.x, this->y * other.y); }
	inline Vector2d operator*(const float& other) const { return Vector2d(this->x * other, this->y * other); }
	inline Vector2d operator/(const Vector2d& other) const { return Vector2d(this->x / other.x, this->y / other.y); }
	inline Vector2d operator/(const float& other) const { return Vector2d(this->x / other, this->y / other); }

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

class Vector3d
{
public:
	float x = 0, y = 0, z = 0;

	Vector3d(float x, float y, float z);
	Vector3d();

	/// <summary>
	/// sets x, y, z to zero
	/// </summary>
	void Clear();
	/// <summary>
	/// returns magnitude of this vector
	/// </summary>
	float Magnitude() const;
	/// <summary>
	/// returns magnitude of vector from parameters
	/// </summary>
	static float Magnitude(const Vector3d& vec);
	/// <summary>
	/// returns normalized vector of this vector
	/// </summary>
	Vector3d Normalize() const;
	/// <summary>
	/// returns normalized vector of vector from parameters
	/// </summary>
	static Vector3d Normalize(const Vector3d& vec);
	/// <summary>
	/// returns dot of this vector and vector from parameters
	/// </summary>
	float Dot(const Vector3d& vec2);
	/// <summary>
	/// returns dot of vectors from parameters
	/// </summary>
	static float Dot(const Vector3d& vec1, const Vector3d& vec2);

	inline Vector3d operator+(const Vector3d& other) const { return Vector3d(this->x + other.x, this->y + other.y, this->z + other.z); }
	inline Vector3d operator-(const Vector3d& other) const { return Vector3d(this->x - other.x, this->y - other.y, this->z - other.z); }
	inline Vector3d operator*(const Vector3d& other) const { return Vector3d(this->x * other.x, this->y * other.y, this->z * other.z); }
	inline Vector3d operator*(const float& other) const { return Vector3d(this->x * other, this->y * other, this->z * other); }
	inline Vector3d operator/(const Vector3d& other) const { return Vector3d(this->x / other.x, this->y / other.y, this->z / other.z); }
	inline Vector3d operator/(const float& other) const { return Vector3d(this->x / other, this->y / other, this->z / other); }

	/// <summary>
	/// 0, 0, 0
	/// </summary>
	static const Vector3d ZERO;
	/// <summary>
	/// 1, 1, 1
	/// </summary>
	static const Vector3d ONE;
	/// <summary>
	/// 0, 0, 1
	/// </summary>
	static const Vector3d FORWARD;
	/// <summary>
	/// 0, 0, -1
	/// </summary>
	static const Vector3d BACK;
	/// <summary>
	/// 0, 1, 0
	/// </summary>
	static const Vector3d UP;
	/// <summary>
	/// 0, -1, 0
	/// </summary>
	static const Vector3d DOWN;
	/// <summary>
	/// 1, 0, 0
	/// </summary>
	static const Vector3d RIGHT;
	/// <summary>
	/// -1, 0, 0
	/// </summary>
	static const Vector3d LEFT;
};