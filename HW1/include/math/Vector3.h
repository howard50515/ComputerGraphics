#pragma once

#include <iosfwd>

namespace hw1 {

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3();
	Vector3(float xVal, float yVal, float zVal);

	float length() const;
	float lengthSquared() const;

	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator*(float scalar) const;
	Vector3 operator/(float scalar) const;
};

std::istream& operator>>(std::istream& in, Vector3& value);
std::ostream& operator<<(std::ostream& out, const Vector3& value);

}  // namespace hw1
