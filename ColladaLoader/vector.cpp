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
 * 線形補間
 * @param out 出力
 * @param v1 頂点1
 * @param v2 頂点2
 * @param t 重み係数
 */
Vector3* Vector3Lerp(Vector3* out, const Vector3* v1, const Vector3* v2, float t){
	out->x = v1->x + (v2->x - v1->x) * t;
	out->y = v1->y + (v2->y - v1->y) * t;
	out->z = v1->z + (v2->z - v1->z) * t;
	return out;
}

/**
 * 重心座標計算
 * @param out 出力
 * @param v1 頂点1
 * @param v2 頂点2
 * @param v3 頂点3
 * @param f v2に対する重み係数
 * @param g v3に対する重み係数
 * (1-f-g)はv1に対する加重
 * (f>=0 && g>=0 && 1-f-g>=0)の場合、点は三角形v1v2v3内にある。
 * (f==0 && g>=0 && 1-f-g>=0)の場合、点は線v1v3上にある。
 * (f>=0 && g==0 && 1-f-g>=0)の場合、点は線v1v2上にある。
 * (f>=0 && g>=0 && 1-f-g==0)の場合、点は線v2v3上にある。
 */
Vector3* Vector3BaryCentric(Vector3* out, const Vector3* v1, const Vector3* v2, const Vector3* v3, float f, float g){
	out->x = v1->x + f * (v2->x - v1->x) + g * (v3->x - v1->x);
	out->y = v1->y + f * (v2->y - v1->y) + g * (v3->y - v1->y);
	out->z = v1->z + f * (v2->z - v1->z) + g * (v3->z - v1->z);
	return out;
}

} // namespace mathematics