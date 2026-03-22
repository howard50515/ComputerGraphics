#include <istream>
#include <ostream>

#include "core/Color.h"

namespace hw1 {

std::istream& operator>>(std::istream& in, Color& value) {
	in >> value.r >> value.g >> value.b;
	return in;
}

std::ostream& operator<<(std::ostream& out, const Color& value) {
	out << value.r << ' ' << value.g << ' ' << value.b;
	return out;
}

}  // namespace hw1