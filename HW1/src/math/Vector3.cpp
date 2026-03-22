#include "math/Vector3.h"

#include <cmath>
#include <istream>
#include <ostream>

namespace hw1 {

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

float Vector3::length() const {
	return std::sqrt(lengthSquared());
}

float Vector3::lengthSquared() const {
	return x * x + y * y + z * z;
}

Vector3 Vector3::operator+(const Vector3& rhs) const {
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const {
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator*(float scalar) const {
	return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
	return Vector3(x / scalar, y / scalar, z / scalar);
}

std::istream& operator>>(std::istream& in, Vector3& value) {
	in >> value.x >> value.y >> value.z;
	return in;
}

std::ostream& operator<<(std::ostream& out, const Vector3& value) {
	out << value.x << ' ' << value.y << ' ' << value.z;
	return out;
}

}  // namespace hw1
