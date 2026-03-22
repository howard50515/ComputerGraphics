#pragma once

#include <cstddef>
#include <vector>

#include "core/Triangle.h"

namespace hw1 {

class Mesh {
public:
	void addTriangle(const Triangle& triangle);
	std::size_t triangleCount() const;

	const std::vector<Triangle>& getTriangles() const;

private:
	std::vector<Triangle> triangles_;
};

}  // namespace hw1
