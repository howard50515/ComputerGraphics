#pragma once

// A container that bundles all uniform variables needed by shaders.
// Provides access to scene data and transformation matrices.
#include "uniform/SceneUniforms.h"
#include "uniform/TransformUniforms.h"

namespace hw1 {

struct UniformContext {
    SceneUniforms scene;
    TransformUniforms transform;
};

} // namespace hw1
