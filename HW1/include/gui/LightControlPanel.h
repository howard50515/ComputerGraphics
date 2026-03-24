#pragma once

#include <cstddef>
#include <string>

#include "core/Color.h"
#include "math/Vector3.h"

namespace hw1 {

class LightControlPanel {
public:
	static std::string renderHtml(std::size_t index, const Vector3& position, const Color& color, float intensity, bool visible);
};

}  // namespace hw1
