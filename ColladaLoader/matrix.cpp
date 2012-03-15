#include "quaternion.h"
#include "matrix.h"

namespace mathematics{

void Matrix44::set(const Quaternion& q){
	const float xx2 = q.x * q.x * 2.0f;
	const float yy2 = q.y * q.y * 2.0f;
	const float zz2 = q.z * q.z * 2.0f;
	const float xy2 = q.x * q.y * 2.0f;
	const float wz2 = q.w * q.z * 2.0f;
	const float xz2 = q.x * q.z * 2.0f;
	const float wy2 = q.w * q.y * 2.0f;
	const float yz2 = q.y * q.z * 2.0f;
	const float wx2 = q.w * q.x * 2.0f;

	m[0] = 1.0f - yy2 - zz2;
	m[1] = xy2 - wz2;
	m[2] = xz2 + wy2;

	m[4] = xy2 + wz2;
	m[5] = 1.0f - xx2 - zz2;
	m[6] = yz2 - wx2;

	m[8] = xz2 - wy2;
	m[9] = yz2 + wx2;
	m[10] = 1.0f - xx2 - yy2;
	
	m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;
}

Matrix44& Matrix44::operator += (const Matrix44& m){
	for(size_t i = 0; i < 16; i++)
		this->m[i] += m.m[i];
	return *this;
}

Matrix44& Matrix44::operator -= (const Matrix44& m){
	for(size_t i = 0; i < 16; i++)
		this->m[i] -= m.m[i];
	return *this;
}

Matrix44& Matrix44::operator *= (const Matrix44& m){
	*this = *this * m;
	return *this;
}

Matrix44& Matrix44::operator *= (float s){
	for(size_t i = 0; i < 16; i++)
		m[i] *= s;
	return *this;
}

const Matrix44 Matrix44::operator + (const Matrix44& m) const {
	Matrix44 ret(*this);
	for(size_t i = 0; i < 16; i++)
		ret.m[i] += m.m[i];
	return ret;
}

const Matrix44 Matrix44::operator - (const Matrix44& m) const {
	Matrix44 ret(*this);
	for(size_t i = 0; i < 16; i++)
		ret.m[i] -= m.m[i];
	return ret;
}

const Matrix44 Matrix44::operator * (const Matrix44& m) const {
	Matrix44 ret;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			ret(i,j) = 0.0f;
			for(int k = 0; k < 4; k++){
				ret(i,j) += (*this)(i,k) * m(k,j);
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
	for(size_t i = 0; i < 16; i++){
		if(fabsf(this->m[i] - m.m[i]) >= EPSILON)
			return false;
	}
	return true;
}

bool Matrix44::operator != (const Matrix44& m) const {
	for(size_t i = 0; i < 16; i++){
		if(fabsf(this->m[i] - m.m[i]) >= EPSILON)
			return true;
	}
	return false;
}

Matrix44* Matrix44Zero(Matrix44* out){
	for(size_t i = 0; i < 16; i++)
		out->m[i] = 0.0f;
	return out;
}

Matrix44* Matrix44Identity(Matrix44* out){
	for(size_t i = 0; i < 16; i++)
		out->m[i] = 0.0f;
	out->m[0] = out->m[5] = out->m[10] = out->m[15] = 1.0f;
	return out;
}

Matrix44* Matrix44Add(Matrix44* out, const Matrix44* m1, const Matrix44* m2){
	for(size_t i = 0; i < 16; i++)
		out->m[i] = m1->m[i] + m2->m[i];
	return out;
}

Matrix44* Matrix44Sub(Matrix44* out, const Matrix44* m1, const Matrix44* m2){
	for(size_t i = 0; i < 16; i++)
		out->m[i] = m1->m[i] - m2->m[i];
	return out;
}

Matrix44* Matrix44Mul(Matrix44* out, const Matrix44* m1, const Matrix44* m2){
	Matrix44 ret;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			ret(i,j) = 0.0f;
			for(int k = 0; k < 4; k++){
				ret(i,j) += (*m1)(i,k) * (*m2)(k,j);
			}
		}
	}
	*out = ret;
	return out;
}

Matrix44* Matrix44Scale(Matrix44* out, const Matrix44* m, float s){
	for(size_t i = 0; i < 16; i++)
		out->m[i] = m->m[i] * s;
	return out;
}

Matrix44* Matrix44Transpose(Matrix44* out, const Matrix44* m){
	Matrix44 temp;
	Matrix44* p;
	p = (out == m)? &temp : out; 
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			(*p)(i,j) = (*m)(j,i);
	// overwrite matrix if needed
	if(p == &temp)
		*out = temp;
	return out;
}

/**
 * ŠÈˆÕ
 */
Matrix44* Matrix44Inverse(Matrix44* out, const Matrix44* m){
	Matrix44 temp;
	Matrix44* p;
	p = (out == m)? &temp : out; 
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			(*p)(i,j) = (*m)(j,i);

#ifdef COLUMN_MAJOR
	(*p)(3,0) = (*p)(3,1) = (*p)(3,2) = 0.0f;
	(*p)(0,3) = -((*m)(0,3) * (*p)(0,0) + (*m)(1,3) * (*p)(0,1) + (*m)(2,3) * (*p)(0,2));
	(*p)(1,3) = -((*m)(0,3) * (*p)(1,0) + (*m)(1,3) * (*p)(1,1) + (*m)(2,3) * (*p)(1,2));
	(*p)(2,3) = -((*m)(0,3) * (*p)(2,0) + (*m)(1,3) * (*p)(2,1) + (*m)(2,3) * (*p)(2,2));
#else
	(*p)(0,3) = (*p)(1,3) = (*p)(2,3) = 0.0f;
	(*p)(3,0) = -((*m)(3,0) * (*p)(0,0) + (*m)(3,1) * (*p)(1,0) + (*m)(3,2) * (*p)(2,0));
	(*p)(3,1) = -((*m)(3,0) * (*p)(0,1) + (*m)(3,1) * (*p)(1,1) + (*m)(3,2) * (*p)(2,1));
	(*p)(3,2) = -((*m)(3,0) * (*p)(0,2) + (*m)(3,1) * (*p)(1,2) + (*m)(3,2) * (*p)(2,2));
#endif
	(*p)(3,3) = 1.0f;
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

} // namespace mathematics 