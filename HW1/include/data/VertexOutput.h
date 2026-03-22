#pragma once

// Represents the output of the vertex shader.
// Contains the transformed clip-space position and varying variables
// that will be interpolated across the triangle.
#include "math/Vector4.h"
#include "data/Varyings.h"

namespace hw1 {

struct VertexOutput {
    Vector4 clipPosition;
    Varyings varyings;
};

} // namespace hw1
