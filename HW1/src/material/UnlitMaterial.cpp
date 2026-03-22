#include "material/UnlitMaterial.h"

#include "math/MathUtils.h"

namespace hw1 {

UnlitMaterial::UnlitMaterial(const Color& baseColor)
	: baseColor_(baseColor) {}

void UnlitMaterial::setBaseColor(const Color& color) {
	baseColor_ = color;
}

const Color& UnlitMaterial::baseColor() const {
	return baseColor_;
}

VertexOutput UnlitMaterial::vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const {
	VertexOutput out;
	const Vector4 localPosition(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f);
	const Vector4 worldPosition4 = ctx.transform.model * localPosition;
	const Vector4 localNormal(vertex.normal.x, vertex.normal.y, vertex.normal.z, 0.0f);
	const Vector4 worldNormal4 = ctx.transform.model * localNormal;
	out.clipPosition = ctx.transform.MVP * localPosition;
	out.varyings.normal = Normalize(Vector3(worldNormal4.x, worldNormal4.y, worldNormal4.z));
	out.varyings.worldPosition = Vector3(worldPosition4.x, worldPosition4.y, worldPosition4.z);
	out.varyings.uv = Vector3(0.0f, 0.0f, 0.0f);
	out.varyings.frontColor = frontColor;
	out.varyings.backColor = backColor;
	return out;
}

Color UnlitMaterial::fragmentShader(const Fragment& fragment, const UniformContext& ctx) const {
	const Vector3 n = Normalize(fragment.varyings.normal);
	const Vector3 viewDir = Normalize(ctx.scene.cameraPosition - fragment.varyings.worldPosition);
	const float facing = Dot(n, viewDir);
	if (facing >= 0.0f) {
		return fragment.varyings.frontColor;
	}
	return fragment.varyings.backColor;
}

}  // namespace hw1
