#include "material/FlatMaterial.h"

#include <algorithm>
#include <cmath>

#include "math/MathUtils.h"

namespace hw1 {

namespace {

Color EvaluatePhongLighting(const Vector3& normal, const Vector3& worldPosition, const Color& baseColorIn, const UniformContext& ctx) {
	const Vector3 baseColor(baseColorIn.r, baseColorIn.g, baseColorIn.b);
	const float ambientStrength = 0.15f;
	const float specularStrength = 0.35f;
	const float shininess = 32.0f;

	const Vector3 n = Normalize(normal);
	const Vector3 viewDir = Normalize(ctx.scene.cameraPosition - worldPosition);

	Vector3 accum(ambientStrength * baseColor.x,
		          ambientStrength * baseColor.y,
		          ambientStrength * baseColor.z);

	for (const Light& light : ctx.scene.lights) {
		const Vector3 lightColor(light.color.r, light.color.g, light.color.b);
		const Vector3 l = Normalize(light.position - worldPosition);
		const float diff = std::max(0.0f, Dot(n, l));

		const Vector3 reflectDir = Normalize(n * (2.0f * Dot(n, l)) - l);
		const float specAngle = std::max(0.0f, Dot(viewDir, reflectDir));
		const float spec = std::pow(specAngle, shininess);

		const float intensity = std::max(0.0f, light.intensity);
		accum.x += (baseColor.x * diff + specularStrength * spec) * lightColor.x * intensity;
		accum.y += (baseColor.y * diff + specularStrength * spec) * lightColor.y * intensity;
		accum.z += (baseColor.z * diff + specularStrength * spec) * lightColor.z * intensity;
	}

	return Color(MathUtils::Clamp01(accum.x), MathUtils::Clamp01(accum.y), MathUtils::Clamp01(accum.z));
}

}  // namespace

VertexOutput FlatMaterial::vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const {
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

void FlatMaterial::prepareTriangle(std::array<VertexOutput, 3>& triangle, const UniformContext& ctx) const {
	(void)ctx;
	const Vector3 p0 = triangle[0].varyings.worldPosition;
	const Vector3 p1 = triangle[1].varyings.worldPosition;
	const Vector3 p2 = triangle[2].varyings.worldPosition;

	Vector3 faceNormal = Cross(p1 - p0, p2 - p0);
	if (faceNormal.lengthSquared() <= 1e-12f) {
		faceNormal = triangle[0].varyings.normal + triangle[1].varyings.normal + triangle[2].varyings.normal;
	}
	faceNormal = Normalize(faceNormal);
	const Vector3 faceCenter = (p0 + p1 + p2) / 3.0f;

	for (std::size_t i = 0u; i < 3u; ++i) {
		triangle[i].varyings.normal = faceNormal;
		triangle[i].varyings.worldPosition = faceCenter;
	}
}

Color FlatMaterial::fragmentShader(const Fragment& fragment, const UniformContext& ctx) const {
	const Vector3 flatNormal = Normalize(fragment.varyings.normal);
	const Vector3 viewDir = Normalize(ctx.scene.cameraPosition - fragment.varyings.worldPosition);
	const bool useFrontColor = Dot(flatNormal, viewDir) >= 0.0f;
	const Color baseColor = useFrontColor ? fragment.varyings.frontColor : fragment.varyings.backColor;
	return EvaluatePhongLighting(flatNormal, fragment.varyings.worldPosition, baseColor, ctx);
}

}  // namespace hw1
