#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

namespace mathematics{

#ifdef COLUMN_MAJOR
#define I(i, j) ((j)*4+(i))
#else
#define I(i, j) ((i)*4+(j))
#endif

void Matrix44::set(const Quaternion& q){
	Matrix44FromQuaternion(this, &q);
}

Matrix44& Matrix44::operator += (const Matrix44& m){
	float* a = this->m;
	const float* ma = m.m;
	for(size_t i = 0; i < 16; i++)
		a[i] += ma[i];
	return *this;
}

Matrix44& Matrix44::operator -= (const Matrix44& m){
	float* a = this->m;
	const float* ma = m.m;
	for(size_t i = 0; i < 16; i++)
		a[i] -= ma[i];
	return *this;
}

Matrix44& Matrix44::operator *= (const Matrix44& m){
	*this = *this * m;
	return *this;
}

Matrix44& Matrix44::operator *= (float s){
	float* a = m;
	for(size_t i = 0; i < 16; i++)
		a[i] *= s;
	return *this;
}

const Matrix44 Matrix44::operator + (const Matrix44& m) const {
	Matrix44 ret(*this);
	const float* ma = m.m;
	for(size_t i = 0; i < 16; i++)
		ret.m[i] += ma[i];
	return ret;
}

const Matrix44 Matrix44::operator - (const Matrix44& m) const {
	Matrix44 ret(*this);
	const float* ma = m.m;
	for(size_t i = 0; i < 16; i++)
		ret.m[i] -= ma[i];
	return ret;
}

const Matrix44 Matrix44::operator * (const Matrix44& m) const {
	Matrix44 ret;
	const float* a = this->m;
	const float* ma = m.m;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			ret.m[I(i,j)] = 0.0f;
			for(int k = 0; k < 4; k++){
				ret.m[(i,j)] += a[I(i,k)] * ma[I(k,j)];
			}
		}
	}
	return ret;
}

const Matrix44 Matrix44::operator * (float s) const {
	Matrix44 ret(*this);
	for(size_t i = 0; i < 16; i++)
		ret.m[i] *= s;
	return ret;
}

const Matrix44 operator * (float s, const Matrix44& m){
	Matrix44 ret(m);
	for(size_t i = 0; i < 16; i++)
		ret.m[i] *= s;
	return ret;
}

bool Matrix44::operator == (const Matrix44& m) const {
	const float* a = this->m;
	const float* ma = m.m;
	for(size_t i = 0; i < 16; i++){
		if(fabsf(a[i] - ma[i]) >= EPSILON)
			return false;
	}
	return true;
}

bool Matrix44::operator != (const Matrix44& m) const {
	const float* a = this->m;
	const float* ma = m.m;
	for(size_t i = 0; i < 16; i++){
		if(fabsf(a[i] - ma[i]) >= EPSILON)
			return true;
	}
	return false;
}

Matrix44* Matrix44Zero(Matrix44* out){
	float* a = out->m;
	for(size_t i = 0; i < 16; i++)
		a[i] = 0.0f;
	return out;
}

Matrix44* Matrix44Identity(Matrix44* out){
	float* a = out->m;
	for(size_t i = 0; i < 16; i++)
		a[i] = 0.0f;
	a[0] = a[5] = a[10] = a[15] = 1.0f;
	return out;
}

Matrix44* Matrix44Add(Matrix44* out, const Matrix44* m1, const Matrix44* m2){
	float* a = out->m;
	const float* m1a = m1->m;
	const float* m2a = m2->m;
	for(size_t i = 0; i < 16; i++)
		a[i] = m1a[i] + m2a[i];
	return out;
}

Matrix44* Matrix44Sub(Matrix44* out, const Matrix44* m1, const Matrix44* m2){
	float* a = out->m;
	const float* m1a = m1->m;
	const float* m2a = m2->m;
	for(size_t i = 0; i < 16; i++)
		a[i] = m1a[i] - m2a[i];
	return out;
}

Matrix44* Matrix44Mul(Matrix44* out, const Matrix44* m1, const Matrix44* m2){
	Matrix44 ret;
	const float* m1a = m1->m;
	const float* m2a = m2->m;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			ret.m[I(i,j)] = 0.0f;
			for(int k = 0; k < 4; k++){
				ret.m[I(i,j)] += m1a[I(i,k)] * m2a[I(k,j)];
			}
		}
	}
	*out = ret;
	return out;
}

Matrix44* Matrix44Scale(Matrix44* out, const Matrix44* m, float s){
	float* a = out->m;
	const float* ma = m->m;
	for(size_t i = 0; i < 16; i++)
		a[i] = ma[i] * s;
	return out;
}

Matrix44* Matrix44Transpose(Matrix44* out, const Matrix44* m){
	Matrix44 temp;
	Matrix44* p;
	p = (out == m)? &temp : out; 

	float* pa = p->m;
	const float* ma = m->m;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			pa[I(i,j)] = ma[I(j,i)];
	// overwrite matrix if needed
	if(p == &temp)
		*out = temp;
	return out;
}

/**
 * 簡易
 */
Matrix44* Matrix44Inverse(Matrix44* out, const Matrix44* m){
	Matrix44 temp;
	Matrix44* p;
	p = (out == m)? &temp : out; 

	float* pa = p->m;
	const float* ma = m->m;

	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			pa[I(i,j)] = ma[I(j,i)];

#ifdef COLUMN_MAJOR
	pa[I(3,0)] = pa[I(3,1)] = pa[I(3,2)] = 0.0f;
	pa[I(0,3)] = -(ma[I(0,3)] * pa[I(0,0)] + ma[I(1,3)] * pa[I(0,1)] + ma[I(2,3)] * pa[I(0,2)]);
	pa[I(1,3)] = -(ma[I(0,3)] * pa[I(1,0)] + ma[I(1,3)] * pa[I(1,1)] + ma[I(2,3)] * pa[I(1,2)]);
	pa[I(2,3)] = -(ma[I(0,3)] * pa[I(2,0)] + ma[I(1,3)] * pa[I(2,1)] + ma[I(2,3)] * pa[I(2,2)]);
#else
	pa[I(0,3)] = pa[I(1,3)] = pa[I(2,3)] = 0.0f;
	pa[I(3,0)] = -(ma[I(3,0)] * pa[I(0,0)] + ma[I(3,1)] * pa[I(1,0)] + ma[I(3,2)] * pa[I(2,0)]);
	pa[I(3,1)] = -(ma[I(3,0)] * pa[I(0,1)] + ma[I(3,1)] * pa[I(1,1)] + ma[I(3,2)] * pa[I(2,1)]);
	pa[I(3,2)] = -(ma[I(3,0)] * pa[I(0,2)] + ma[I(3,1)] * pa[I(1,2)] + ma[I(3,2)] * pa[I(2,2)]);
#endif
	pa[I(3,3)] = 1.0f;
	// overwrite matrix if needed
	if(p == &temp)
		*out = temp;
	return out;
}

Matrix44* Matrix44Translation(Matrix44* out, float x, float y, float z){
	Matrix44Identity(out);
#ifdef COLUMN_MAJOR
	out->m03 = x;
	out->m13 = y;
	out->m23 = z;
#else
	out->m30 = x;
	out->m31 = y;
	out->m32 = z;
#endif
	return out;
}

Matrix44* Matrix44RotationX(Matrix44* out, float angle){
	Matrix44Identity(out);
	const float theta = ToRadian(angle);
	const float s = sinf(theta);
	const float c = cosf(theta);
#ifdef COLUMN_MAJOR
	out->m11 = c; out->m12 =-s;
	out->m21 = s; out->m22 = c;
#else
	out->m11 = c; out->m12 = s;
	out->m21 =-s; out->m22 = c;
#endif
	return out;
}

Matrix44* Matrix44RotationY(Matrix44* out, float angle){
	Matrix44Identity(out);
	const float theta = ToRadian(angle);
	const float s = sinf(theta);
	const float c = cosf(theta);
#ifdef COLUMN_MAJOR
	out->m00 = c; out->m02 = s;
	out->m20 =-s; out->m22 = c;
#else
	out->m00 = c; out->m02 =-s;
	out->m20 = s; out->m22 = c;
#endif
	return out;
}

Matrix44* Matrix44RotationZ(Matrix44* out, float angle){
	Matrix44Identity(out);
	const float theta = ToRadian(angle);
	const float s = sinf(theta);
	const float c = cosf(theta);
#ifdef COLUMN_MAJOR
	out->m00 = c; out->m01 =-s;
	out->m10 = s; out->m11 = c;
#else
	out->m00 = c; out->m01 = s;
	out->m10 =-s; out->m11 = c;
#endif
	return out;
}

/**
 * クォータニオンから行列へ
 * 対角成分は直接アクセスでいいかも
 */
Matrix44* Matrix44FromQuaternion(Matrix44* out, const Quaternion* q){
	const float xx2 = q->x * q->x * 2.0f;
	const float yy2 = q->y * q->y * 2.0f;
	const float zz2 = q->z * q->z * 2.0f;
	const float xy2 = q->x * q->y * 2.0f;
	const float wz2 = q->w * q->z * 2.0f;
	const float xz2 = q->x * q->z * 2.0f;
	const float wy2 = q->w * q->y * 2.0f;
	const float yz2 = q->y * q->z * 2.0f;
	const float wx2 = q->w * q->x * 2.0f;

	float* a = out->m;
	a[I(0,0)] = 1.0f - yy2 - zz2;
	a[I(1,0)] = xy2 - wz2;
	a[I(2,0)] = xz2 + wy2;

	a[I(0,1)] = xy2 + wz2;
	a[I(1,1)] = 1.0f - xx2 - zz2;
	a[I(2,1)] = yz2 - wx2;

	a[I(0,2)] = xz2 - wy2;
	a[I(1,2)] = yz2 + wx2;
	a[I(2,2)] = 1.0f - xx2 - yy2;
	// 直接アクセスでいいかも
	a[I(3,0)] = a[I(3,1)] = a[I(3,2)] = a[I(0,3)] = a[I(1,3)] = a[I(2,3)] = 0.0f;
	a[I(3,3)] = 1.0f;
	return out;
}

/**
 * 行列からクォータニオンへ
 * 対角成分は直接アクセスでいいかも
 */
Quaternion* Matrix44ToQuaternion(Quaternion* out, const Matrix44* m){
	const float* a = m->m;
	const float tr = m->trace();
	// 一般的にm44に相当する成分は1.0のため加算する必要はない
	// その場合、Trの比較は0.0で行うことができる
	// ※ただし、sqrt(tr + 1) で計算
	// 現在は適用していない
	if(tr >= 1.0f){	// |w| >= 0.5 と等価
		// w成分が最も大きい場合
		const float s = 0.5f / sqrtf(tr); // 1/4qw
		out->w = 0.25f / s;
		out->x = (a[I(2,1)] - a[I(1,2)]) * s;
		out->y = (a[I(0,2)] - a[I(2,0)]) * s;
		out->z = (a[I(1,0)] - a[I(0,1)]) * s;
	}
	else
	if((a[I(0,0)] >= a[I(1,1)]) && (a[I(0,0)] >= a[I(2,2)])){
		// x成分が最も大きい場合
		const float s = 0.5f / sqrtf(1.0f + a[I(0,0)] - a[I(1,1)] - a[I(2,2)]); // 1/4qx
		out->w = (a[I(2,1)] - a[I(1,2)]) * s;
		out->x = 0.25f / s;
		out->y = (a[I(0,1)] + a[I(1,0)]) * s;
		out->z = (a[I(0,2)] + a[I(2,0)]) * s;
	}
	else
	if(a[I(1,1)] >= a[I(2,2)]){
		// y成分が最も大きい場合
		const float s = 0.5f / sqrtf(1.0f + a[I(1,1)] - a[I(0,0)] - a[I(2,2)]); // 1/4qy
		out->w = (a[I(0,2)] - a[I(2,0)]) * s;
		out->x = (a[I(0,1)] + a[I(1,0)]) * s;
		out->y = 0.25f / s;
		out->z = (a[I(1,2)] + a[I(2,1)]) * s;
	}
	else{
		// z成分が最も大きい
		const float s = 0.5f / sqrtf(1.0f + a[I(2,2)] - a[I(0,0)] - a[I(1,1)]); // 1/4qz
		out->w = (a[I(1,0)] - a[I(0,1)]) * s;
		out->x = (a[I(0,2)] + a[I(2,0)]) * s;
		out->y = (a[I(1,2)] + a[I(2,1)]) * s;
		out->z = 0.25f / s;
	}
	return out;
}

Matrix44* Matrix44LookAt(Matrix44* out, class Vector3* position, const Vector3* target, const Vector3* up){
	float*a = out->m;
	Vector3 zaxis;
#ifdef RIGHT_HANDED_SYSTEM
	// for OpenGL
	Vector3Sub(&zaxis, position, target);
#else
	// for Direct3D
	Vector3Sub(&zaxis, target, position);
#endif
#ifdef _DEBUG
	if(zaxis.length() < EPSILON){
	}
#endif
	Vector3Normalize(&zaxis, &zaxis);

	Vector3 yaxis;
	Vector3Scale(&yaxis, &zaxis, Vector3InnerProduct(up, &zaxis));
	Vector3Sub(&yaxis, up, &yaxis);
	if(yaxis.length() < EPSILON){
		Vector3 temp(0.0f, 1.0f, 0.0f);
		Vector3Scale(&yaxis, &zaxis, zaxis.y);
		Vector3Sub(&yaxis, &temp, &yaxis);
		if(yaxis.length() < EPSILON){
			temp.set(0.0f, 0.0f, 1.0f);
			Vector3Scale(&yaxis, &zaxis, zaxis.z);
			Vector3Sub(&yaxis, &temp, &yaxis);
#ifdef _DEBUG
			if(yaxis.length() < EPSILON){
			}
#endif
		}
	}
	Vector3Normalize(&yaxis, &yaxis);

	Vector3 xaxis;
	Vector3OuterProduct(&xaxis, &yaxis, &zaxis);

#ifdef COLUMN_MAJOR
	a[I(0,0)] = xaxis.x; a[I(0,1)] = xaxis.y; a[I(0,2)] = xaxis.z; a[I(0,3)] = -Vector3InnerProduct(&xaxis, position); 
	a[I(1,0)] = yaxis.x; a[I(1,1)] = yaxis.y; a[I(1,2)] = yaxis.z; a[I(1,3)] = -Vector3InnerProduct(&yaxis, position);
	a[I(2,0)] = zaxis.x; a[I(2,1)] = zaxis.y; a[I(2,2)] = zaxis.z; a[I(2,3)] = -Vector3InnerProduct(&zaxis, position);
	a[I(3,0)] = 0.0f;
	a[I(3,1)] = 0.0f;
	a[I(3,2)] = 0.0f;
	a[I(3,3)] = 1.0f;
#else
	a[I(0,0)] = xaxis.x; a[I(0,1)] = yaxis.x; a[I(0,2)] = zaxis.x; a[I(0,3)] = 0.0f;
	a[I(1,0)] = xaxis.y; a[I(1,1)] = yaxis.y; a[I(1,2)] = zaxis.y; a[I(1,3)] = 0.0f;
	a[I(2,0)] = xaxis.z; a[I(2,1)] = yaxis.z; a[I(2,2)] = zaxis.z; a[I(2,3)] = 0.0f;
	a[I(3,0)] = -Vector3InnerProduct(&xaxis, position);
	a[I(3,1)] = -Vector3InnerProduct(&yaxis, position);
	a[I(3,2)] = -Vector3InnerProduct(&zaxis, position);
	a[I(3,3)] = 1.0f;
#endif
	return NULL;
}

/**
 * 視錐体
 * @param t top
 * @param b bottom
 * @param l left
 * @param r right
 * @param n near plane
 * @param f far plane
 */
Matrix44* Matrix44Frustum(Matrix44* out, float t, float b, float l, float r, float n, float f){
	// assert(t != b)
	// assert(l != r)
	// assert(n != f)
	Matrix44Zero(out);
	float*a = out->m;
#ifdef RIGHT_HANDED_SYSTEM
	// for OpenGL
	const float w = 2.0f * n / (r - l);
	const float h = 2.0f * n / (t - b);
	const float q = 1.0f / (f - n);
	const float woff = (r + l) / (r - l);
	const float hoff = (t + b) / (t - b);

	a[I(0,0)] = w;
	a[I(1,1)] = h;
	a[I(0,2)] = woff;
	a[I(1,2)] = hoff;
	a[I(2,2)] = -(f + n) * q; 
	a[I(2,3)] = -2.0f * f * n * q;
	a[I(3,2)] = -1.0f;
#else
	// for Direct3D
	const float w = 2.0f * n / (r - l);
	const float h = 2.0f * n / (t - b);
	const float q = f / (f - n);
	const float woff = -(r + l) / (r - l);
	const float hoff = -(t + b) / (t - b);

	a[I(0,0)] = w;
	a[I(1,1)] = h;
	a[I(2,0)] = woff;
	a[I(2,1)] = hoff;
	a[I(2,2)] = q;
	a[I(2,3)] = 1.0f;
	a[I(3,2)] = -n * q;
#endif
	return out;
}

/**
 * 正射影
 * @param t top
 * @param b bottom
 * @param l left
 * @param r right
 * @param n near plane
 * @param f far plane
 */
Matrix44* Matrix44Ortho(Matrix44* out, float t, float b, float l, float r, float n, float f){
	// assert(t != b)
	// assert(l != r)
	// assert(n != f)
	Matrix44Zero(out);
	float*a = out->m;
#ifdef RIGHT_HANDED_SYSTEM
	// for OpenGL
	const float w = 2.0f / (r - l);
	const float h = 2.0f / (t - b);
	const float q = 1.0f / (f - n);
	const float woff = -(r + l) / (r - l);
	const float hoff = -(t + b) / (t - b);

	a[I(0,0)] = w;
	a[I(1,1)] = h;
	a[I(2,2)] = -2.0f * q;
	a[I(0,3)] = woff;
	a[I(1,3)] = hoff;
	a[I(2,3)] = -(f + n) * q;
	a[I(3,3)] = 1.0f;
#else
	// for Direct3D
	const float w = 2.0f / (r - l);
	const float h = 2.0f / (t - b);
	const float q = 1.0f / (f - n);
	const float woff = -(r + l) / (r - l);
	const float hoff = -(t + b) / (t - b);

	a[I(0,0)] = w;
	a[I(1,1)] = h;
	a[I(2,2)] = q;
	a[I(3,0)] = woff;
	a[I(3,1)] = hoff;
	a[I(3,2)] = -n * q;
	a[I(3,3)] = 1.0f;
#endif
	return out;
}

/**
 * 射影
 * @param fovy field of view
 * @param aspect aspect ratio
 * @param n near plane
 * @param f far plane
 */
Matrix44* Matrix44Perspective(Matrix44* out, float fovy, float aspect, float n, float f){
	// assert((fovy > 0.0f) && (fovy < 180.0f))
	// assert(aspect > 0.0f)
	// assert(n != f)
	Matrix44Zero(out);
	float*a = out->m;
#ifdef RIGHT_HANDED_SYSTEM
	// for OpenGL
	const float hfovy = ToRadian(fovy * 0.5f);
	const float cot = 1.0f / tanf(hfovy);
	const float q = 1.0f / (f - n);

	a[I(0,0)] = cot / aspect;
	a[I(1,1)] = cot;
	a[I(2,2)] = -(f + n) * q;
	a[I(2,3)] = -2.0f * f * n * q;
	a[I(3,2)] = -1.0f;
#else
	// for Direct3D
	const float hfovy = ToRadian(fovy * 0.5f);
	const float cot = 1.0f / tanf(hfovy);
	const float q = f / (f - n);

	a[I(0,0)] = cot / aspect;
	a[I(1,1)] = cot;
	a[I(2,2)] = q;
	a[I(2,3)] = 1.0f;
	a[I(3,2)] = -n * q;
#endif
	return out;
}

} // namespace mathematics 