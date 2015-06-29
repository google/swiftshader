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

#ifndef Matrix_hpp
#define Matrix_hpp

namespace sw
{
	struct Vector;
	struct Point;
	struct float4;

	struct Matrix
	{
		Matrix();
		Matrix(const int i);
		Matrix(const float m[16]);
		Matrix(const float m[4][4]);
		Matrix(float m11, float m12, float m13,
		       float m21, float m22, float m23,
		       float m31, float m32, float m33);
		Matrix(float m11, float m12, float m13, float m14,
		       float m21, float m22, float m23, float m24,
		       float m31, float m32, float m33, float m34,
		       float m41, float m42, float m43, float m44);
		Matrix(const Vector &v1, const Vector &v2, const Vector &v3);   // Column vectors

		Matrix &operator=(const Matrix &N);

		// Row major order
		float m[4][4];

		static Matrix diag(float m11, float m22, float m33, float m44);

		operator float*();

		Matrix operator+() const;
		Matrix operator-() const;

		Matrix operator!() const;   // Inverse
		Matrix operator~() const;   // Transpose

		Matrix &operator+=(const Matrix &N);
		Matrix &operator-=(const Matrix &N);
		Matrix &operator*=(float s);
		Matrix &operator*=(const Matrix &N);
		Matrix &operator/=(float s);

		float *operator[](int i);   // Access element [row][col], starting with [0][0]
		const float *operator[](int i) const;

		float &operator()(int i, int j);   // Access element (row, col), starting with (1, 1)
		const float &operator()(int i, int j) const;

		friend bool operator==(const Matrix &M, const Matrix &N);
		friend bool operator!=(const Matrix &M, const Matrix &N);

		friend Matrix operator+(const Matrix &M, const Matrix &N);
		friend Matrix operator-(const Matrix &M, const Matrix &N);
		friend Matrix operator*(float s, const Matrix &M);
		friend Matrix operator*(const Matrix &M, const Matrix &N);
		friend Matrix operator/(const Matrix &M, float s);

		float4 operator*(const float4 &v) const;

		static float det(const Matrix &M);
		static float det(float m11);
		static float det(float m11, float m12,
		                 float m21, float m22);
		static float det(float m11, float m12, float m13,
		                 float m21, float m22, float m23,
		                 float m31, float m32, float m33);
		static float det(float m11, float m12, float m13, float m14,
		                 float m21, float m22, float m23, float m24,
		                 float m31, float m32, float m33, float m34,
		                 float m41, float m42, float m43, float m44);
		static float det(const Vector &v1, const Vector &v2, const Vector &v3);
		static float det3(const Matrix &M);

		static float tr(const Matrix &M);

		Matrix &orthogonalise();   // Gram-Schmidt orthogonalisation of 3x3 submatrix

		static Matrix eulerRotate(const Vector &v);
		static Matrix eulerRotate(float x, float y, float z);
		
		static Matrix translate(const Vector &v);
		static Matrix translate(float x, float y, float z);
		
		static Matrix scale(const Vector &v);
		static Matrix scale(float x, float y, float z);

		static Matrix lookAt(const Vector &v);
		static Matrix lookAt(float x, float y, float z);
	};
}

#include "Vector.hpp"

namespace sw
{
	inline Matrix::Matrix()
	{
	}

	inline Matrix::Matrix(const int i)
	{
		const float s = (float)i;

		Matrix &M = *this;

		M(1, 1) = s; M(1, 2) = 0; M(1, 3) = 0; M(1, 4) = 0;
		M(2, 1) = 0; M(2, 2) = s; M(2, 3) = 0; M(2, 4) = 0;
		M(3, 1) = 0; M(3, 2) = 0; M(3, 3) = s; M(3, 4) = 0;
		M(4, 1) = 0; M(4, 2) = 0; M(4, 3) = 0; M(4, 4) = s;
	}

	inline Matrix::Matrix(const float m[16])
	{
		Matrix &M = *this;

		M(1, 1) = m[0];  M(1, 2) = m[1];  M(1, 3) = m[2];  M(1, 4) = m[3];
		M(2, 1) = m[4];  M(2, 2) = m[5];  M(2, 3) = m[6];  M(2, 4) = m[7];
		M(3, 1) = m[8];  M(3, 2) = m[8];  M(3, 3) = m[10]; M(3, 4) = m[11];
		M(4, 1) = m[12]; M(4, 2) = m[13]; M(4, 3) = m[14]; M(4, 4) = m[15];
	}

	inline Matrix::Matrix(const float m[4][4])
	{
		Matrix &M = *this;

		M[0][0] = m[0][0];  M[0][1] = m[0][1];  M[0][2] = m[0][2];  M[0][3] = m[0][3];
		M[1][0] = m[1][0];  M[1][1] = m[1][1];  M[1][2] = m[1][2];  M[1][3] = m[1][3];
		M[2][0] = m[2][0];  M[2][1] = m[2][1];  M[2][2] = m[2][2];  M[2][3] = m[2][3];
		M[3][0] = m[3][0];  M[3][1] = m[3][1];  M[3][2] = m[3][2];  M[3][3] = m[3][3];
	}

	inline Matrix::Matrix(float m11, float m12, float m13, 
	                      float m21, float m22, float m23, 
	                      float m31, float m32, float m33)
	{
		Matrix &M = *this;

		M(1, 1) = m11; M(1, 2) = m12; M(1, 3) = m13; M(1, 4) = 0;
		M(2, 1) = m21; M(2, 2) = m22; M(2, 3) = m23; M(2, 4) = 0;
		M(3, 1) = m31; M(3, 2) = m32; M(3, 3) = m33; M(3, 4) = 0;
		M(4, 1) = 0;   M(4, 2) = 0;   M(4, 3) = 0;   M(4, 4) = 1;
	}

	inline Matrix::Matrix(float m11, float m12, float m13, float m14, 
	                      float m21, float m22, float m23, float m24, 
	                      float m31, float m32, float m33, float m34, 
	                      float m41, float m42, float m43, float m44)
	{
		Matrix &M = *this;

		M(1, 1) = m11; M(1, 2) = m12; M(1, 3) = m13; M(1, 4) = m14;
		M(2, 1) = m21; M(2, 2) = m22; M(2, 3) = m23; M(2, 4) = m24;
		M(3, 1) = m31; M(3, 2) = m32; M(3, 3) = m33; M(3, 4) = m34;
		M(4, 1) = m41; M(4, 2) = m42; M(4, 3) = m43; M(4, 4) = m44;
	}

	inline Matrix::Matrix(const Vector &v1, const Vector &v2, const Vector &v3)
	{
		Matrix &M = *this;

		M(1, 1) = v1.x; M(1, 2) = v2.x; M(1, 3) = v3.x; M(1, 4) = 0;
		M(2, 1) = v1.y; M(2, 2) = v2.y; M(2, 3) = v3.y; M(2, 4) = 0;
		M(3, 1) = v1.z; M(3, 2) = v2.z; M(3, 3) = v3.z; M(3, 4) = 0;
		M(4, 1) = 0;    M(4, 2) = 0;    M(4, 3) = 0;    M(4, 4) = 1;
	}

	inline Matrix &Matrix::operator=(const Matrix &N)
	{
		Matrix &M = *this;

		M(1, 1) = N(1, 1); M(1, 2) = N(1, 2); M(1, 3) = N(1, 3); M(1, 4) = N(1, 4);
		M(2, 1) = N(2, 1); M(2, 2) = N(2, 2); M(2, 3) = N(2, 3); M(2, 4) = N(2, 4);
		M(3, 1) = N(3, 1); M(3, 2) = N(3, 2); M(3, 3) = N(3, 3); M(3, 4) = N(3, 4);
		M(4, 1) = N(4, 1); M(4, 2) = N(4, 2); M(4, 3) = N(4, 3); M(4, 4) = N(4, 4);

		return M;
	}

	inline float *Matrix::operator[](int i)
	{
		return m[i];
	}

	inline const float *Matrix::operator[](int i) const
	{
		return m[i];
	}

	inline float &Matrix::operator()(int i, int j)
	{
		return m[i - 1][j - 1];
	}

	inline const float &Matrix::operator()(int i, int j) const
	{
		return m[i - 1][j - 1];
	}
}

#endif   // Matrix_hpp
