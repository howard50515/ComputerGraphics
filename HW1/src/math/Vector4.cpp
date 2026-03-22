#include "math/Vector4.h"

#include <istream>
#include <ostream>

namespace hw1 {

Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

Vector4::Vector4(float xVal, float yVal, float zVal, float wVal)
	: x(xVal), y(yVal), z(zVal), w(wVal) {}

std::istream& operator>>(std::istream& in, Vector4& value) {
	in >> value.x >> value.y >> value.z >> value.w;
	return in;
}

std::ostream& operator<<(std::ostream& out, const Vector4& value) {
	out << value.x << ' ' << value.y << ' ' << value.z << ' ' << value.w;
	return out;
}

}  // namespace hw1
