#include "core/Vertex.h"

namespace hw1 {

Vertex::Vertex() = default;

Vertex::Vertex(const Vector3& p, const Vector3& n)
	: position(p), normal(n) {}

}  // namespace hw1
