#pragma once

#include "core/Vertex.h"
#include "core/Color.h"

namespace hw1 {

struct Triangle {
	Vertex v0;
	Vertex v1;
	Vertex v2;

	Color frontColor;
	Color backColor;

	Triangle();
};

}  // namespace hw1
