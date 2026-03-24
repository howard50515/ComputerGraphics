#pragma once

#include "math/Matrix4x4.h"
#include "math/Vector3.h"

namespace hw1 {

class Transform {
public:
	static Matrix4x4 translation(const Vector3& t);
	static Matrix4x4 scale(const Vector3& s);
	static Matrix4x4 shear(float xy, float xz, float yx, float yz, float zx, float zy);
	static Matrix4x4 rotationX(float radians);
	static Matrix4x4 rotationY(float radians);
	static Matrix4x4 rotationZ(float radians);
};

}  // namespace hw1
