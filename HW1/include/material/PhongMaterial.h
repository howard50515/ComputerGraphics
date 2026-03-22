#pragma once

// Implements Phong shading.
// Computes lighting per fragment using interpolated normals and supports multiple lights.

#include "material/Material.h"

namespace hw1 {

class PhongMaterial : public Material {
public:
	VertexOutput vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const override;
	Color fragmentShader(const Fragment& fragment, const UniformContext& ctx) const override;
};

}  // namespace hw1
