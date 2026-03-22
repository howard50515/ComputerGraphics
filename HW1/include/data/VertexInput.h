#pragma once

// Defines the input attributes of a vertex.
// Typically includes position, normal, and texture coordinates.

#include "math/Vector3.h"

namespace hw1 {

struct VertexInput {
    Vector3 position;
    Vector3 normal;
    Vector3 uv;
};

}   // namespace hw1