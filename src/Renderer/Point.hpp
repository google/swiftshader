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

#ifndef Point_hpp
#define Point_hpp

namespace sw
{
	struct Vector;
	struct Matrix;

	struct Point
	{
		Point();
		Point(const int i);
		Point(const Point &P);
		Point(const Vector &v);
		Point(float Px, float Py, float Pz);

		Point &operator=(const Point &P);

		union
		{
			float p[3];

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

		Point &operator+=(const Vector &v);
		Point &operator-=(const Vector &v);

		friend Point operator+(const Point &P, const Vector &v);
		friend Point operator-(const Point &P, const Vector &v);

		friend Vector operator-(const Point &P, const Point &Q);

		friend Point operator*(const Matrix &M, const Point& P);
		friend Point operator*(const Point &P, const Matrix &M);
		friend Point &operator*=(Point &P, const Matrix &M);

		float d(const Point &P) const;   // Distance between two points
		float d2(const Point &P) const;   // Squared distance between two points

		static float d(const Point &P, const Point &Q);   // Distance between two points
		static float d2(const Point &P, const Point &Q);   // Squared distance between two points
	};
}

#include "Vector.hpp"

namespace sw
{
	inline Point::Point()
	{
	}

	inline Point::Point(const int i)
	{
		const float s = (float)i;

		x = s;
		y = s;
		z = s;
	}

	inline Point::Point(const Point &P)
	{
		x = P.x;
		y = P.y;
		z = P.z;
	}

	inline Point::Point(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline Point::Point(float P_x, float P_y, float P_z)
	{
		x = P_x;
		y = P_y;
		z = P_z;
	}

	inline Point &Point::operator=(const Point &P)
	{
		x = P.x;
		y = P.y;
		z = P.z;

		return *this;
	}

	inline float &Point::operator()(int i)
	{
		return p[i];
	}

	inline float &Point::operator[](int i)
	{
		return p[i];
	}

	inline const float &Point::operator()(int i) const
	{
		return p[i];
	}

	inline const float &Point::operator[](int i) const
	{
		return p[i];
	}
}

#endif   // Point_hpp
