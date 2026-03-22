#pragma once

// Represents a complete shader program.
// Contains a vertex shader and a fragment shader that define the shading behavior.
#include "shader/VertexShader.h"
#include "shader/FragmentShader.h"

namespace hw1 {

class Shader {
public:
    Shader(VertexShader* v, FragmentShader* f);

    VertexShader* vertex;
    FragmentShader* fragment;
};

} // namespace hw1
