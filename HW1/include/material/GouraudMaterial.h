#pragma once

#include "material/Material.h"

namespace hw1 {

class GouraudMaterial : public Material {
public:
	VertexOutput vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const override;
	Color fragmentShader(const Fragment& fragment, const UniformContext& ctx) const override;
};

}  // namespace hw1
