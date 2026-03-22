#pragma once

#include "math/Vector4.h"

namespace hw1 {

struct Matrix4x4 {
	float m[4][4];

	Matrix4x4();

	static Matrix4x4 identity();

	Matrix4x4 operator*(const Matrix4x4& rhs) const;
	Vector4 operator*(const Vector4& vec) const;
};

}  // namespace hw1
