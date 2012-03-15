#pragma once
#include "mathematics.h"

namespace mathematics{

class Vector3{
public:
	// constructor's
	Vector3(){ set(0.0f, 0.0f, 0.0f); }
	Vector3(const Vector3& v){ set(v.a); }
	Vector3(float x, float y, float z){ set(x, y, z); }
	Vector3(const float* a){ set(a); }
	// setter
	inline void set(const Vector3& v){ x = v.x; y = v.y; z = v.z; }
	inline void set(float x, float y, float z){ this->x = x; this->y = y; this->z = z; }
	inline void set(const float* a){ this->a[0] = a[0]; this->a[1] = a[1]; this->a[2] = a[2]; }
	// casting
	operator float* (){ return a; }
	operator const float* () const { return reinterpret_cast<const float*>(a); }
	// assignment operator's
	Vector3& operator = (const Vector3& v){ set(v); return *this; }
	Vector3& operator = (const float* v){ set(v); return *this; }
	Vector3& operator += (const Vector3& v){ x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator -= (const Vector3& v){ x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator *= (float s){ x *= s; y *= s; z *= s; return *this; }
	Vector3& operator /= (float s){ x /= s; y /= s; z /= s; return *this; }
	// unary operators
	const Vector3 operator + () const { return *this; }
	const Vector3 operator - () const { return Vector3(-x, -y, -z); }
	// binary operator's
	const Vector3 operator + (const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	const Vector3 operator - (const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	const Vector3 operator * (float s) const { return Vector3(x * s, y * s, z * s); }
	const Vector3 operator / (float s) const { return Vector3(x / s, y / s, z / s); }
	friend const Vector3 operator * (float s, const Vector3& v);
	bool operator == (const Vector3& v) const {
		if(fabsf(x - v.x) >= EPSILON) return false;
		if(fabsf(y - v.y) >= EPSILON) return false;
		if(fabsf(z - v.z) >= EPSILON) return false;
		return true;
	}
	bool operator != (const Vector3& v) const {
		if(fabsf(x - v.x) >= EPSILON) return true;
		if(fabsf(y - v.y) >= EPSILON) return true;
		if(fabsf(z - v.z) >= EPSILON) return true;
		return false;
	}
	bool operator < (const Vector3& v) const {
		const float l = x*x+y*y+z*z;
		const float r = v.x*v.x+v.y*v.y+v.z*v.z;
		if(fabsf(l - r) < EPSILON)
			return false;
		return l < r;
	}
	bool operator > (const Vector3& v) const {
		const float l = x*x+y*y+z*z;
		const float r = v.x*v.x+v.y*v.y+v.z*v.z;
		if(fabsf(l - r) < EPSILON)
			return false;
		return l > r;
	}
	bool operator <= (const Vector3& v) const {
		const float l = x*x+y*y+z*z;
		const float r = v.x*v.x+v.y*v.y+v.z*v.z;
		if(fabsf(l - r) < EPSILON)
			return true;
		return l < r;
	}
	bool operator >= (const Vector3& v) const {
		const float l = x*x+y*y+z*z;
		const float r = v.x*v.x+v.y*v.y+v.z*v.z;
		if(fabsf(l - r) < EPSILON)
			return true;
		return l > r;
	}
	// operation's
	float length() const { return sqrtf(x*x+y*y+z*z); }
	float lengthSq() const { return  x*x+y*y+z*z; }
public:
	union{
		struct{
			float x;
			float y;
			float z;
		};
		float a[3];
	};
};

// operation's
Vector3* Vector3Add(Vector3* out, const Vector3* v1, const Vector3* v2);
Vector3* Vector3Sub(Vector3* out, const Vector3* v1, const Vector3* v2);
Vector3* Vector3Scale(Vector3* out, const Vector3* v, float s);
Vector3* Vector3Minimize(Vector3* out, const Vector3* v1, const Vector3* v2);
Vector3* Vector3Maximize(Vector3* out, const Vector3* v1, const Vector3* v2);
Vector3* Vector3Normalize(Vector3* out, const Vector3* v);
Vector3* Vector3OuterProduct(Vector3* out, const Vector3* v1, const Vector3* v2);
float Vector3InnerProduct(const Vector3* v1, const Vector3* v2);
Vector3* Vector3Lerp(Vector3* out, const Vector3* v1, const Vector3* v2, float t);
Vector3* Vector3BaryCentric(Vector3* out, const Vector3* v1, const Vector3* v2, const Vector3* v3, float f, float g);

} // namespace mathematics