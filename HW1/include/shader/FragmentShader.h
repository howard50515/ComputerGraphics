#pragma once

// Interface for fragment shader implementations.
// Computes the final pixel color using interpolated varyings, lights, and material data.
#include "data/Fragment.h"
#include "math/Vector4.h"
#include "uniform/UniformContext.h"

namespace hw1 {

class FragmentShader {
public:
    virtual ~FragmentShader() = default;

    virtual Vector4 run(
        const Fragment& fragment,
        const UniformContext& ctx) const = 0;
};

} // namespace hw1
