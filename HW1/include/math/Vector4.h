#pragma once

#include <iosfwd>

namespace hw1 {

struct Vector4 {
	float x;
	float y;
	float z;
	float w;

	Vector4();
	Vector4(float xVal, float yVal, float zVal, float wVal);
};

std::istream& operator>>(std::istream& in, Vector4& value);
std::ostream& operator<<(std::ostream& out, const Vector4& value);

}  // namespace hw1
