#pragma once

// Clips triangles against the viewing frustum.
// Removes or splits triangles that fall outside the camera view volume.

#include <array>
#include <vector>

#include "data/VertexOutput.h"

namespace hw1 {

class Clipper {
public:
	using TriangleOut = std::array<VertexOutput, 3>;

	std::vector<TriangleOut> clip(const TriangleOut& triangle) const;
};

}  // namespace hw1
