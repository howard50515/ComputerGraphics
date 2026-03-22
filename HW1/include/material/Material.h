#pragma once

// Base class for all materials.
// Defines the interface for vertex and fragment shading functions.
// Derived materials implement different shading models.
#include <array>

#include "data/Fragment.h"
#include "data/VertexOutput.h"
#include "core/Color.h"
#include "core/Vertex.h"
#include "uniform/UniformContext.h"

namespace hw1 {

class Material {
public:
    virtual ~Material() = default;

    virtual VertexOutput vertexShader(
        const Vertex& vertex,
        const Color& frontColor,
        const Color& backColor,
        const UniformContext& ctx) const = 0;

    virtual Color fragmentShader(
        const Fragment& fragment,
        const UniformContext& ctx) const = 0;

    virtual void prepareTriangle(
        std::array<VertexOutput, 3>& triangle,
        const UniformContext& ctx) const {
        (void)triangle;
        (void)ctx;
    }
};

} // namespace hw1
