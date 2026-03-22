#pragma once

#include "material/Material.h"

namespace hw1 {

class UnlitMaterial : public Material {
public:
	explicit UnlitMaterial(const Color& baseColor = Color(1.0f, 1.0f, 1.0f));

	void setBaseColor(const Color& color);
	const Color& baseColor() const;

	VertexOutput vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const override;
	Color fragmentShader(const Fragment& fragment, const UniformContext& ctx) const override;

private:
	Color baseColor_;
};

}  // namespace hw1
