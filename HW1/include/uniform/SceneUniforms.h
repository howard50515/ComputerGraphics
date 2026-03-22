#pragma once

// Stores scene-wide uniform data shared across shaders.
// Includes lights and camera position.
#include <vector>
#include "core/Light.h"
#include "math/Vector3.h"

namespace hw1 {

struct SceneUniforms {
    std::vector<Light> lights;
    Vector3 cameraPosition;
};

} // namespace hw1

