#pragma once

#include "math/Vector3.h"

namespace hw1 {

struct Vertex {
	Vector3 position;
	Vector3 normal;

	Vertex();
	Vertex(const Vector3& p, const Vector3& n);
};

}  // namespace hw1
