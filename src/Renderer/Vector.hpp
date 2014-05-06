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

#ifndef Vector_hpp
#define Vector_hpp

namespace sw
{
	struct Point;
	struct Matrix;
	struct Plane;

	struct Vector
	{
		Vector();
		Vector(const int i);
		Vector(const Vector &v);
		Vector(const Point &p);
		Vector(float v_x, float v_y, float v_z);

		Vector &operator=(const Vector &v);

		union
		{
			float v[3];

			struct
			{
				float x;
				float y;
				float z;
			};
		};

		float &operator[](int i);
		float &operator()(int i);

		const float &operator[](int i) const;
		const float &operator()(int i) const;

		Vector operator+() const;
		Vector operator-() const;

		Vector &operator+=(const Vector &v);
		Vector &operator-=(const Vector &v);
		Vector &operator*=(float s);
		Vector &operator/=(float s);

		friend bool operator==(const Vector &u, const Vector &v);
		friend bool operator!=(const Vector &u, const Vector &v);

		friend Vector operator+(const Vector &u, const Vector &v);
		friend Vector operator-(const Vector &u, const Vector &v);
		friend float operator*(const Vector &u, const Vector &v);   // Dot product
		friend Vector operator*(float s, const Vector &v);
		friend Vector operator*(const Vector &v, float s);
		friend Vector operator/(const Vector &v, float s);
		friend float operator^(const Vector &u, const Vector &v);   // Angle between vectors
		friend Vector operator%(const Vector &u, const Vector &v);   // Cross product

		friend Vector operator*(const Matrix &M, const Vector& v);
		friend Vector operator*(const Vector &v, const Matrix &M);
		friend Vector &operator*=(Vector &v, const Matrix &M);

		static float N(const Vector &v);   // Norm
		static float N2(const Vector &v);   // Squared norm

		static Vector mirror(const Vector &v, const Plane &p);
		static Vector reflect(const Vector &v, const Plane &p);
		static Vector lerp(const Vector &u, const Vector &v, float t);
	};
}

#include "Point.hpp"

namespace sw
{
	inline Vector::Vector()
	{
	}

	inline Vector::Vector(const int i)
	{
		const float s = (float)i;

		x = s;
		y = s;
		z = s;
	}

	inline Vector::Vector(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline Vector::Vector(const Point &P)
	{
		x = P.x;
		y = P.y;
		z = P.z;
	}

	inline Vector::Vector(float v_x, float v_y, float v_z)
	{
		x = v_x;
		y = v_y;
		z = v_z;
	}

	inline Vector &Vector::operator=(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	inline float &Vector::operator()(int i)
	{
		return v[i];
	}

	inline float &Vector::operator[](int i)
	{
		return v[i];
	}

	inline const float &Vector::operator()(int i) const
	{
		return v[i];
	}

	inline const float &Vector::operator[](int i) const
	{
		return v[i];
	}
}

#endif   // Vector_hpp
