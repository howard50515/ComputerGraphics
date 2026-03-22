#pragma once

// Interface for vertex shader implementations.
// Responsible for transforming vertices from object space to clip space
// and generating varying data for interpolation.
#include "data/VertexInput.h"
#include "data/VertexOutput.h"
#include "uniform/UniformContext.h"

namespace hw1 {

class VertexShader {
public:
    virtual ~VertexShader() = default;

    virtual VertexOutput run(
        const VertexInput& input,
        const UniformContext& ctx) const = 0;
};

} // namespace hw1
