#include "vector.h"
#include "quaternion.h"

namespace mathematics{

const Quaternion operator * (float s, const Quaternion& q){
	return Quaternion(q.w * s, q.x * s, q.y * s, q.z * s);
}

Quaternion* QuaternionAdd(Quaternion* out, const Quaternion* q1, const Quaternion* q2){
	out->w = q1->w + q2->w;
	out->x = q1->x + q2->x;
	out->y = q1->y + q2->y;
	out->z = q1->z + q2->z;
	return out;
}

Quaternion* QuaternionSub(Quaternion* out, const Quaternion* q1, const Quaternion* q2){
	out->w = q1->w - q2->w;
	out->x = q1->x - q2->x;
	out->y = q1->y - q2->y;
	out->z = q1->z - q2->z;
	return out;
}

Quaternion* QuaternionScale(Quaternion* out, const Quaternion* q, float s){
	out->w = q->w * s;
	out->x = q->x * s;
	out->y = q->y * s;
	out->z = q->z * s;
	return out;
}

/**
 * 乗算
 * @param out 出力
 * @param q1 クォータニオン1
 * @param q2 クォータニオン2
 * q1 = (s1, v1)
 * q2 = (s2, v2)
 * q = q1 * q2 = (s1 * s2 - v1・v2, s1 * v2 + s2 * v1 + v1 x v2)
 */
Quaternion* QuaternionMul(Quaternion* out, const Quaternion* q1, const Quaternion* q2){
	const float w = q1->w * q2->w - (q1->x * q2->x + q1->y * q2->y + q1->z * q2->z);
	const float x = q1->w * q2->x + q2->w * q1->x + (q1->y * q2->z - q1->z * q2->y);
	const float y = q1->w * q2->y + q2->w * q1->y + (q1->z * q2->x - q1->x * q2->z);
	const float z = q1->w * q2->z + q2->w * q1->z + (q1->x * q2->y - q1->y * q2->x);
	out->w = w;
	out->x = x;
	out->y = y;
	out->z = z;
	return out;
}

/**
 * 共役
 * @param out 出力
 * @param q クォータニオン
 * q = (w, -v)
 */
Quaternion* QuaternionConjugate(Quaternion* out, const Quaternion* q){
	out->w = q->w;
	out->x = -q->x;
	out->y = -q->y;
	out->z = -q->z;
	return out;
}

/**
 * 任意軸回転
 * @param out 出力
 * @param axis 回転軸
 * @param angle 角度
 * q = (s, v) = {cos(θ/2), axis*sin(θ/2)}
 */
Quaternion* QuaternionRotation(Quaternion* out, const Vector3* axis, float angle){
#ifdef RIGHT_HANDED_SYSTEM
	angle = -angle;
#endif
	const float r = ToRadian(angle) * 0.5f;
	const float s = sinf(r);
	out->w = cosf(r);
	out->x = axis->x * s;
	out->y = axis->y * s;
	out->z = axis->z * s;
	return out;
}

/**
 * 最小弧クォータニオン
 * @param out 出力
 * @param v1 正規化ベクトル1
 * @param v2 正規化ベクトル2
 */
Quaternion* QuaternionRotationArc(Quaternion* out, const Vector3* nv1, const Vector3* nv2){
	const float d = Vector3InnerProduct(nv1, nv2);
	const float s = sqrtf((1.0f + d) * 2.0f);
	Vector3 c;
	Vector3OuterProduct(&c, nv1, nv2);
	out->w = s * 0.5f;
	out->x = c.x / s;
	out->y = c.y / s;
	out->z = c.z / s;
	return out;
}

/**
 * 正規化
 * @param out 出力
 * @param q クォータニオン
 */
Quaternion* QuaternionNormalize(Quaternion* out, const Quaternion* q){
	const float n = q->norm();
#ifdef _DEBUG
	if(n < EPSILON){
	}
#endif
	return QuaternionScale(out, out, 1.0f/n);
}

/**
 * 線形補間
 * @param out 出力
 * @param q1 クォータニオン1
 * @param q2 クォータニオン2
 * @param t 重み係数(t: 0 <= t <= 1)
 */
Quaternion* QuaternionLerp(Quaternion* out, const Quaternion* q1, const Quaternion* q2, float t){
	out->w = q1->w + t * (q2->w - q1->w); 
	out->x = q1->x + t * (q2->x - q1->x); 
	out->y = q1->y + t * (q2->y - q1->y); 
	out->z = q1->z + t * (q2->z - q1->z); 
	return QuaternionNormalize(out, out);
}

/**
 * 球面線形補間
 * @param out 出力
 * @param q1 クォータニオン1
 * @param q2 クォータニオン2
 * @param t 重み係数(t: 0 <= t <= 1)
 */
Quaternion* QuaternionSlerp(Quaternion* out, const Quaternion* q1, const Quaternion* q2, float t){
	const float theta = acosf(QuaternionInnerProduct(q1, q2));
	const float rs = 1.0f / sinf(theta);
	const float w1 = sinf((1.0f - t) * theta) * rs;
	const float w2 = sinf(t * theta) * rs;
	out->w = w1 * q1->w + w2 * q2->w;
	out->x = w1 * q1->x + w2 * q2->x;
	out->y = w1 * q1->y + w2 * q2->y;
	out->z = w1 * q1->z + w2 * q2->z;
	return out;
}

float QuaternionInnerProduct(const Quaternion* q1, const Quaternion* q2){
	return q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z;
}

} // namespace mathematics