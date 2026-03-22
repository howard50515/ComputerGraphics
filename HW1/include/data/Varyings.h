#pragma once

// Stores values passed from vertex shader to fragment shader.
// These values are interpolated during rasterization.
#include "core/Color.h"
#include "math/Vector3.h"

namespace hw1 {

struct Varyings {
    Vector3 normal;
    Vector3 worldPosition;
    Vector3 uv;
    Color frontColor;
    Color backColor;
};

} // namespace hw1
