#include "math/MathUtils.h"

#include <algorithm>
#include <cmath>

namespace hw1 {

float MathUtils::Clamp01(float v) {
	return std::max(0.0f, std::min(1.0f, v));
}

float Dot(const Vector3& a, const Vector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Cross(const Vector3& a, const Vector3& b) {
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

Vector3 Normalize(const Vector3& v) {
	const float len = std::sqrt(Dot(v, v));
	if (len <= 1e-6f) {
		return Vector3();
	}
	return v / len;
}

}  // namespace hw1
