#include "core/Mesh.h"

namespace hw1 {

void Mesh::addTriangle(const Triangle& triangle) {
	triangles_.push_back(triangle);
}

std::size_t Mesh::triangleCount() const {
	return triangles_.size();
}

const std::vector<Triangle>& Mesh::getTriangles() const {
	return triangles_;
}

}  // namespace hw1
