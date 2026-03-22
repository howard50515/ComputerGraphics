#pragma once

// Stores transformation matrices such as model, view, projection,
// and the combined MVP matrix used in vertex transformations.
#include "math/Matrix4x4.h"

namespace hw1 {

struct TransformUniforms {
    Matrix4x4 model;
    Matrix4x4 view;
    Matrix4x4 projection;
    Matrix4x4 MVP;
};

} // namespace hw1