#pragma once

#include "math/Vector3.h"

namespace hw1 {

inline constexpr float PI = 3.14159265359f;

class MathUtils {
public:
	static float Clamp01(float v);
};

float Dot(const Vector3& a, const Vector3& b);
Vector3 Cross(const Vector3& a, const Vector3& b);
Vector3 Normalize(const Vector3& v);

}  // namespace hw1
