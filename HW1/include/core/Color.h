
#pragma once

#include <istream>
#include <ostream>

namespace hw1 {

struct Color {
	float r;
	float g;
	float b;

	Color()
		: r(1.0f),
		  g(1.0f),
		  b(1.0f) {}

	Color(float red, float green, float blue)
		: r(red),
		  g(green),
		  b(blue) {}

};

std::istream& operator>>(std::istream& in, Color& value);
std::ostream& operator<<(std::ostream& out, const Color& value);

}  // namespace hw1
