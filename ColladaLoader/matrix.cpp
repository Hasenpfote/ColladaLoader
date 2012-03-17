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

Matrix44* Matrix44Scaling(Matrix44* out, float x, float y, float z){
	Matrix44Identity(out);
	float* a = out->m;
	a[0] = x;
	a[5] = y;
	a[10] = z;
	return out;
}

Matrix44* Matrix44RotationX(Matrix44* out, float angle){
	Matrix44Identity(out);
	const float theta = ToRadian(angle);
	const float s = sinf(theta);
	const float c = cosf(theta);
#ifdef RIGHT_HANDED_SYSTEM
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
#ifdef RIGHT_HANDED_SYSTEM
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
#ifdef RIGHT_HANDED_SYSTEM
	out->m00 = c; out->m01 =-s;
	out->m10 = s; out->m11 = c;
#else
	out->m00 = c; out->m01 = s;
	out->m10 =-s; out->m11 = c;
#endif
	return out;
}

/**
 * 任意軸回転
 * @param axis 回転軸
 * @param angle 角度
 * (1-cosθ)周辺の展開方法を変えると乗算回数が減る(未対応)
 */
Matrix44* Matrix44RotationAxis(Matrix44* out, const Vector3* axis, float angle){
	const float x = axis->x;
	const float y = axis->y;
	const float z = axis->z;
	const float xy = x * y;
	const float xz = x * z;
	const float yz = y * z;
	angle = ToRadian(angle);
	const float c = cosf(angle);
	const float s = sinf(angle);
	const float omc = 1.0f - c;
	const float xs = x * s;
	const float ys = y * s;
	const float zs = z * s;

	float* a = out->m;
#ifdef RIGHT_HANDED_SYSTEM
	// x^2(1-cosθ)+cosθ,	xy(1-cosθ)-zsinθ,	xz(1-cosθ)+ysinθ
	// xy(1-cosθ)+zsinθ,	y^2(1-cosθ)+cosθ,	yz(1-cosθ)-xsinθ
	// xz(1-cosθ)-ysinθ,	yz(1-cosθ)+xsinθ,	z^2(1-cosθ)+cosθ
	a[I(0,0)] = x * x * omc + c;
	a[I(1,0)] = xy * omc + zs;
	a[I(2,0)] = xz * omc - ys;
	a[I(3,0)] = 0.0f;

	a[I(0,1)] = xy * omc - zs;
	a[I(1,1)] = y * y * omc + c;
	a[I(2,1)] = yz * omc + xs;
	a[I(3,1)] = 0.0f;

	a[I(0,2)] = xz * omc + ys;
	a[I(1,2)] = yz * omc - xs;
	a[I(2,2)] = z * z * omc + c;
	a[I(3,2)] = 0.0f;
#else
	// x^2(1-cosθ)+cosθ,	xy(1-cosθ)+zsinθ,	xz(1-cosθ)-ysinθ
	// xy(1-cosθ)-zsinθ,	y^2(1-cosθ)+cosθ,	yz(1-cosθ)+xsinθ
	// xz(1-cosθ)+ysinθ,	yz(1-cosθ)-xsinθ,	z^2(1-cosθ)+cosθ
	a[I(0,0)] = x * x * omc + c;
	a[I(1,0)] = xy * omc - zs;
	a[I(2,0)] = xz * omc + ys;
	a[I(3,0)] = 0.0f;

	a[I(0,1)] = xy * omc + zs;
	a[I(1,1)] = y * y * omc + c;
	a[I(2,1)] = yz * omc - xs;
	a[I(3,1)] = 0.0f;

	a[I(0,2)] = xz * omc - ys;
	a[I(1,2)] = yz * omc + xs;
	a[I(2,2)] = z * z * omc + c;
	a[I(3,2)] = 0.0f;
#endif
	a[I(0,3)] = 
	a[I(1,3)] = 
	a[I(2,3)] = 0.0f;
	a[I(3,3)] = 1.0f;

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
 * @param t top of view volume at the near clipping plane
 * @param b bottom of view volume at the near clipping plane
 * @param l left of view volume at the near clipping plane
 * @param r right of view volume at the near clipping plane
 * @param n positive distance from camera to near clipping plane
 * @param f positive distance from camera to far clipping plane
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
 * @param t top of parallel view volume
 * @param b bottom of parallel view volume
 * @param l left of parallel view volume
 * @param r right of parallel view volume
 * @param n positive distance from camera to near clipping plane
 * @param f positive distance from camera to far clipping plane
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
 * @param fovy total field of view in the YZ plane
 * @param aspect aspect ration of view window width:height (X / Y)
 * @param n positive distance from camera to near clipping plane
 * @param f positive distance from camera to far clipping plane
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