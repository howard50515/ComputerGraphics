#include "math/Matrix4x4.h"

namespace hw1 {

Matrix4x4::Matrix4x4() : m{{0.0f, 0.0f, 0.0f, 0.0f},
						   {0.0f, 0.0f, 0.0f, 0.0f},
						   {0.0f, 0.0f, 0.0f, 0.0f},
						   {0.0f, 0.0f, 0.0f, 0.0f}} {}

Matrix4x4 Matrix4x4::identity() {
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const {
	Matrix4x4 out;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			float sum = 0.0f;
			for (int k = 0; k < 4; ++k) {
				sum += m[i][k] * rhs.m[k][j];
			}
			out.m[i][j] = sum;
		}
	}
	return out;
}

Vector4 Matrix4x4::operator*(const Vector4& vec) const {
	return Vector4(
		m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.w,
		m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.w,
		m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.w,
		m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.w);
}

}  // namespace hw1
