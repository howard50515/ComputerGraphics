#include "pipeline/Transform.h"

#include <cmath>

namespace hw1 {

Matrix4x4 Transform::translation(const Vector3& t) {
	Matrix4x4 m = Matrix4x4::identity();
	m.m[0][3] = t.x;
	m.m[1][3] = t.y;
	m.m[2][3] = t.z;
	return m;
}

Matrix4x4 Transform::scale(const Vector3& s) {
	Matrix4x4 m = Matrix4x4::identity();
	m.m[0][0] = s.x;
	m.m[1][1] = s.y;
	m.m[2][2] = s.z;
	return m;
}

Matrix4x4 Transform::shear(float xy, float xz, float yx, float yz, float zx, float zy) {
	Matrix4x4 m = Matrix4x4::identity();
	m.m[0][1] = xy;
	m.m[0][2] = xz;
	m.m[1][0] = yx;
	m.m[1][2] = yz;
	m.m[2][0] = zx;
	m.m[2][1] = zy;
	return m;
}

Matrix4x4 Transform::rotationX(float radians) {
	Matrix4x4 m = Matrix4x4::identity();
	const float c = std::cos(radians);
	const float s = std::sin(radians);
	m.m[1][1] = c;
	m.m[1][2] = -s;
	m.m[2][1] = s;
	m.m[2][2] = c;
	return m;
}

Matrix4x4 Transform::rotationY(float radians) {
	Matrix4x4 m = Matrix4x4::identity();
	const float c = std::cos(radians);
	const float s = std::sin(radians);
	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -s;
	m.m[2][2] = c;
	return m;
}

Matrix4x4 Transform::rotationZ(float radians) {
	Matrix4x4 m = Matrix4x4::identity();
	const float c = std::cos(radians);
	const float s = std::sin(radians);
	m.m[0][0] = c;
	m.m[0][1] = -s;
	m.m[1][0] = s;
	m.m[1][1] = c;
	return m;
}

}  // namespace hw1
