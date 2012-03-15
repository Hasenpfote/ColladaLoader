#pragma once
#include "mathematics.h"

namespace mathematics{

class Vector3;

class Quaternion{
public:
	// constructor's
	Quaternion(){ set(1.0f, 0.0f, 0.0f, 0.0f); }
	Quaternion(const Quaternion& q){ set(q.a); }
	Quaternion(float w, float x, float y, float z){ set(w, x, y, z); }
	Quaternion(const float* a){ set(a); }
	// setter
	inline void set(const Quaternion& q){ w = q.w; x = q.x; y = q.y; z = q.z; }
	inline void set(float w, float x, float y, float z){ this->w = w; this->x = x; this->y = y; this->z = z; }
	inline void set(const float* a){ this->a[0] = a[0]; this->a[1] = a[1]; this->a[2] = a[2]; this->a[3] = a[3]; }
	// casting
	operator float* (){ return a; }
	operator const float* () const { return reinterpret_cast<const float*>(a); }
	// assignment operator's
	Quaternion& operator = (const Quaternion& v){ set(v); return *this; }
	Quaternion& operator = (const float* v){ set(v); return *this; }
	// unary operators
	const Quaternion operator + () const { return *this; }
	const Quaternion operator - () const { return Quaternion(-w, -x, -y, -z); }
	// binary operator's
	bool operator == (const Quaternion& q) const {
		if(fabsf(w - q.w) >= EPSILON) return false;
		if(fabsf(x - q.x) >= EPSILON) return false;
		if(fabsf(y - q.y) >= EPSILON) return false;
		if(fabsf(z - q.z) >= EPSILON) return false;
		return true;
	}
	bool operator != (const Quaternion& q) const {
		if(fabsf(w - q.w) >= EPSILON) return true;
		if(fabsf(x - q.x) >= EPSILON) return true;
		if(fabsf(y - q.y) >= EPSILON) return true;
		if(fabsf(z - q.z) >= EPSILON) return true;
		return false;
	}
	// operation's
	float norm() const { return sqrtf(w*w+x*x+y*y+z*z); }
public:
	union{
		struct{
			float w;
			float x;
			float y;
			float z;
		};
		float a[4];
	};
};

// operation's
Quaternion* QuaternionMul(Quaternion* out, const Quaternion* q1, const Quaternion* q2);
Quaternion* QuaternionConjugate(Quaternion* out, const Quaternion* q);
Quaternion* QuaternionRotation(Quaternion* out, const Vector3* axis, float angle);
Quaternion* QuaternionRotationArc(Quaternion* out, const Vector3* nv1, const Vector3* nv2);
Quaternion* QuaternionNormalize(Quaternion* out, const Quaternion* q);
Quaternion* QuaternionLerp(Quaternion* out, const Quaternion* q1, const Quaternion* q2, float t);
Quaternion* QuaternionSlerp(Quaternion* out, const Quaternion* q1, const Quaternion* q2, float t);
float QuaternionInnerProduct(const Quaternion* q1, const Quaternion* q2);

} // namespace mathematics