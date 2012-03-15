#include "vector.h"

namespace mathematics{

const Vector3 operator * (float s, const Vector3& v){
	return Vector3(s * v.x, s * v.y, s * v.z);
}

Vector3* Vector3Add(Vector3* out, const Vector3* v1, const Vector3* v2){
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
	return out;
}

Vector3* Vector3Sub(Vector3* out, const Vector3* v1, const Vector3* v2){
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
	return out;
}

Vector3* Vector3Scale(Vector3* out, const Vector3* v, float s){
	out->x = v->x * s;
	out->y = v->y * s;
	out->z = v->z * s;
	return out;
}

Vector3* Vector3Minimize(Vector3* out, const Vector3* v1, const Vector3* v2){
	out->x = (v1->x > v2->x)? v1->x : v2->x;
	out->y = (v1->y > v2->y)? v1->y : v2->y;
	out->z = (v1->z > v2->z)? v1->z : v2->z;
	return out;
}

Vector3* Vector3Maximize(Vector3* out, const Vector3* v1, const Vector3* v2){
	out->x = (v1->x < v2->x)? v1->x : v2->x;
	out->y = (v1->y < v2->y)? v1->y : v2->y;
	out->z = (v1->z < v2->z)? v1->z : v2->z;
	return out;
}

Vector3* Vector3Normalize(Vector3* out, const Vector3* v){
	float s = v->x*v->x+v->y*v->y+v->z*v->z;
#ifdef _DEBUG
	if(s < EPSILON){
	}
#endif
	return Vector3Scale(out, v, 1.0f/sqrtf(s));
}

Vector3* Vector3OuterProduct(Vector3* out, const Vector3* v1, const Vector3* v2){
	const float x = v1->y * v2->z - v1->z * v2->y;
	const float y = v1->z * v2->x - v1->x * v2->z;
	const float z = v1->x * v2->y - v1->y * v2->x;
	out->x = x;
	out->y = y;
	out->z = z;
	return out;
}

float Vector3InnerProduct(const Vector3* v1, const Vector3* v2){
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

/**
 * ���`���
 * @param out �o��
 * @param v1 ���_1
 * @param v2 ���_2
 * @param t �d�݌W��
 */
Vector3* Vector3Lerp(Vector3* out, const Vector3* v1, const Vector3* v2, float t){
	out->x = v1->x + (v2->x - v1->x) * t;
	out->y = v1->y + (v2->y - v1->y) * t;
	out->z = v1->z + (v2->z - v1->z) * t;
	return out;
}

/**
 * �d�S���W�v�Z
 * @param out �o��
 * @param v1 ���_1
 * @param v2 ���_2
 * @param v3 ���_3
 * @param f v2�ɑ΂���d�݌W��
 * @param g v3�ɑ΂���d�݌W��
 * (1-f-g)��v1�ɑ΂�����d
 * (f>=0 && g>=0 && 1-f-g>=0)�̏ꍇ�A�_�͎O�p�`v1v2v3���ɂ���B
 * (f==0 && g>=0 && 1-f-g>=0)�̏ꍇ�A�_�͐�v1v3��ɂ���B
 * (f>=0 && g==0 && 1-f-g>=0)�̏ꍇ�A�_�͐�v1v2��ɂ���B
 * (f>=0 && g>=0 && 1-f-g==0)�̏ꍇ�A�_�͐�v2v3��ɂ���B
 */
Vector3* Vector3BaryCentric(Vector3* out, const Vector3* v1, const Vector3* v2, const Vector3* v3, float f, float g){
	out->x = v1->x + f * (v2->x - v1->x) + g * (v3->x - v1->x);
	out->y = v1->y + f * (v2->y - v1->y) + g * (v3->y - v1->y);
	out->z = v1->z + f * (v2->z - v1->z) + g * (v3->z - v1->z);
	return out;
}

} // namespace mathematics