#pragma once

#include <cstddef>
#include <string>

#include "math/Vector3.h"

namespace hw1 {

class CameraControlPanel {
public:
	static std::string renderHtml(std::size_t index, const Vector3& position, bool active);
};

}  // namespace hw1
