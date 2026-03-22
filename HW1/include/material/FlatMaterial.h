#pragma once

#include "material/Material.h"

namespace hw1 {

class FlatMaterial : public Material {
public:
	VertexOutput vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const override;
	Color fragmentShader(const Fragment& fragment, const UniformContext& ctx) const override;
	void prepareTriangle(std::array<VertexOutput, 3>& triangle, const UniformContext& ctx) const override;
};

}  // namespace hw1
