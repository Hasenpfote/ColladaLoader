#pragma once
//#define _USE_MATH_DEFINES
#include <math.h>

#define EPSILON 0.00000001f
#define PI 3.141592654f
#define RIGHT_HANDED_SYSTEM	// 厳密にはOpenGLの右手座標系かDirect3Dの左手座標系かの定義のため反転しただけでは利用できない
#define COLUMN_MAJOR
#define ToDegree(radian) ((radian) * (180.0f / PI))
#define ToRadian(degree) ((degree) * (PI / 180.0f))

namespace mathematics{


} // namespace mathematics
