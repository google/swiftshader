// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Vector.hpp"

#include "Math.hpp"
#include "Matrix.hpp"

namespace sw
{
	Vector Vector::operator+() const
	{
		return *this;
	}

	Vector Vector::operator-() const
	{
		return Vector(-x, -y, -z);
	}

	Vector &Vector::operator+=(const Vector &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector &Vector::operator-=(const Vector &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector &Vector::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	Vector &Vector::operator/=(float s)
	{
		float r = 1.0f / s;

		return *this *= r;
	}

	bool operator==(const Vector &U, const Vector &v)
	{
		if(U.x == v.x && U.y == v.y && U.z == v.z)
			return true;
		else
			return false;
	}

	bool operator!=(const Vector &U, const Vector &v)
	{
		if(U.x != v.x || U.y != v.y || U.z != v.z)
			return true;
		else
			return false;
	}

	bool operator>(const Vector &u, const Vector &v)
	{
		if((u^2) > (v^2))
			return true;
		else
			return false;
	}

	bool operator<(const Vector &u, const Vector &v)
	{
		if((u^2) < (v^2))
			return true;
		else
			return false;
	}

	Vector operator+(const Vector &u, const Vector &v)
	{
		return Vector(u.x + v.x, u.y + v.y, u.z + v.z);
	}

	Vector operator-(const Vector &u, const Vector &v)
	{
		return Vector(u.x - v.x, u.y - v.y, u.z - v.z);
	}

	float operator*(const Vector &u, const Vector &v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	Vector operator*(float s, const Vector &v)
	{
		return Vector(s * v.x, s * v.y, s * v.z);
	}

	Vector operator*(const Vector &v, float s)
	{
		return Vector(v.x * s, v.y * s, v.z * s);
	}

	Vector operator/(const Vector &v, float s)
	{
		float r = 1.0f / s;

		return Vector(v.x * r, v.y * r, v.z * r);
	}

	float operator^(const Vector &u, const Vector &v)
	{
		return acos(u / Vector::N(u) * v / Vector::N(v));
	}

	Vector operator%(const Vector &u, const Vector &v)
	{
		return Vector(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
	}

	Vector operator*(const Matrix &M, const Vector &v)
	{
		return Vector(M(1, 1) * v.x + M(1, 2) * v.y + M(1, 3) * v.z,
		              M(2, 1) * v.x + M(2, 2) * v.y + M(2, 3) * v.z,
		              M(3, 1) * v.x + M(3, 2) * v.y + M(3, 3) * v.z);
	}

	Vector operator*(const Vector &v, const Matrix &M)
	{
		return Vector(v.x * M(1, 1) + v.y * M(2, 1) + v.z * M(3, 1) + M(4, 1),
		              v.x * M(1, 2) + v.y * M(2, 2) + v.z * M(3, 2) + M(4, 2),
		              v.x * M(1, 3) + v.y * M(2, 3) + v.z * M(3, 3) + M(4, 3));
	}

	Vector &operator*=(Vector &v, const Matrix &M)
	{
		return v = v * M;
	}

	float Vector::N(const Vector &v)
	{
		return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}

	float Vector::N2(const Vector &v)
	{
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}

	Vector lerp(const Vector &u, const Vector &v, float t)
	{
		return Vector(u.x + t * (v.x - u.x),
		              u.y + t * (v.y - u.y),
					  u.z + t * (v.z - u.x));
	}
}
