#pragma once
#include <stdio.h>
#include "mathematics.h"

namespace mathematics{

class Quaternion;

class Matrix44{
public:
	// constructor's
	Matrix44(){}
	Matrix44(const Matrix44& m){ set(m); }
	Matrix44(const Quaternion& q){ set(q); }
	Matrix44(float m00, float m01, float m02, float m03,
			 float m10, float m11, float m12, float m13,
			 float m20, float m21, float m22, float m23,
			 float m30, float m31, float m32, float m33){
		set(m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33);
	}
	Matrix44(const float* m){
		set(m);
	}
	// setter
	inline void set(const Matrix44& m){
		set(m.m00, m.m01, m.m02, m.m03,
			m.m10, m.m11, m.m12, m.m13,
			m.m20, m.m21, m.m22, m.m23,
			m.m30, m.m31, m.m32, m.m33);
	}
	void set(const Quaternion& q);
	inline void set(float m00, float m01, float m02, float m03,
					float m10, float m11, float m12, float m13,
					float m20, float m21, float m22, float m23,
					float m30, float m31, float m32, float m33){
		this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
		this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
		this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
		this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
	}
	void set(const float* m){
		for(size_t i = 0; i < 16; i++)
			this->m[i] = m[i];
	}
	// casting
	operator float* (){ return m; }
	operator const float* () const { return m; }
	// assignment operator's
	Matrix44& operator = (const Matrix44& m){ set(m); return *this; }
	Matrix44& operator = (const float* m){ set(m); return *this; }
	Matrix44& operator += (const Matrix44& m);
	Matrix44& operator -= (const Matrix44& m);
	Matrix44& operator *= (const Matrix44& m);
	Matrix44& operator *= (float s);
	// binary operator's
	const Matrix44 operator + (const Matrix44& m) const;
	const Matrix44 operator - (const Matrix44& m) const;
	const Matrix44 operator * (const Matrix44& m) const;
	const Matrix44 operator * (float s) const;
	friend const Matrix44 operator * (float s, const Matrix44& m);
	bool operator == (const Matrix44& m) const;
	bool operator != (const Matrix44& m) const;
	// subscript operator's
//	float* operator [] (int i){ return m2[i]; }
//	const float* operator [] (int i) const { return m2[i]; }
	inline float& operator()(int i, int j){
#ifdef COLUMN_MAJOR
		return m[i+j*4];
#else
		return m[i*4+j];
#endif
	}
	inline const float& operator()(int i, int j) const {
#ifdef COLUMN_MAJOR
		return m[i+j*4];
#else
		return m[i*4+j];
#endif
	}

	// operation's
	inline float trace(){ return m00 + m11 + m22 + m33; }
public:
	union{
		struct{
#ifdef COLUMN_MAJOR
			float m00, m10, m20, m30;
			float m01, m11, m21, m31;
			float m02, m12, m22, m32;
			float m03, m13, m23, m33;
#else
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
#endif
		};
		float m[16];
	};

	void dump() const {
		printf("%.10f %.10f %.10f %.10f\n", m00, m01, m02, m03);
		printf("%.10f %.10f %.10f %.10f\n", m10, m11, m12, m13);
		printf("%.10f %.10f %.10f %.10f\n", m20, m21, m22, m23);
		printf("%.10f %.10f %.10f %.10f\n", m30, m31, m32, m33);
	}

};

// operation's
Matrix44* Matrix44Zero(Matrix44* out);
Matrix44* Matrix44Identity(Matrix44* out);
Matrix44* Matrix44Add(Matrix44* out, const Matrix44* m1, const Matrix44* m2);
Matrix44* Matrix44Sub(Matrix44* out, const Matrix44* m1, const Matrix44* m2);
Matrix44* Matrix44Mul(Matrix44* out, const Matrix44* m1, const Matrix44* m2);
Matrix44* Matrix44Scale(Matrix44* out, const Matrix44* m, float s);
Matrix44* Matrix44Transpose(Matrix44* out, const Matrix44* m);
Matrix44* Matrix44Inverse(Matrix44* out, const Matrix44* m);
Matrix44* Matrix44Translation(Matrix44* out, float x, float y, float z);
Matrix44* Matrix44RotationX(Matrix44* out, float angle);
Matrix44* Matrix44RotationY(Matrix44* out, float angle);
Matrix44* Matrix44RotationZ(Matrix44* out, float angle);

} // namespace mathematics