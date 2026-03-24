#include "material/GouraudMaterial.h"

#include <algorithm>
#include <cmath>

#include "config/LightingSettings.h"
#include "math/MathUtils.h"

namespace hw1 {

namespace {

Color EvaluatePhongLighting(const Vector3& normal, const Vector3& worldPosition, const Color& baseColorIn, const UniformContext& ctx) {
	const Vector3 baseColor(baseColorIn.r, baseColorIn.g, baseColorIn.b);
	const LightingSettings& settings = GlobalLightingSettingsConst();
	const float ambientStrength = settings.ambientStrength;
	const float specularStrength = settings.specularStrength;
	const float shininess = settings.shininess;

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

VertexOutput GouraudMaterial::vertexShader(const Vertex& vertex, const Color& frontColor, const Color& backColor, const UniformContext& ctx) const {
	VertexOutput out;
	const Vector4 localPosition(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f);
	const Vector4 worldPosition4 = ctx.transform.model * localPosition;
	out.clipPosition = ctx.transform.MVP * localPosition;

	const Vector4 localNormal(vertex.normal.x, vertex.normal.y, vertex.normal.z, 0.0f);
	const Vector4 worldNormal4 = ctx.transform.model * localNormal;
	out.varyings.normal = Normalize(Vector3(worldNormal4.x, worldNormal4.y, worldNormal4.z));
	out.varyings.worldPosition = Vector3(worldPosition4.x, worldPosition4.y, worldPosition4.z);
	out.varyings.uv = Vector3(0.0f, 0.0f, 0.0f);
	out.varyings.frontColor = EvaluatePhongLighting(out.varyings.normal, out.varyings.worldPosition, frontColor, ctx);
	out.varyings.backColor = EvaluatePhongLighting(out.varyings.normal * -1.0f, out.varyings.worldPosition, backColor, ctx);
	return out;
}

Color GouraudMaterial::fragmentShader(const Fragment& fragment, const UniformContext& ctx) const {
	const Vector3 n = Normalize(fragment.varyings.normal);
	const Vector3 viewDir = Normalize(ctx.scene.cameraPosition - fragment.varyings.worldPosition);
	if (Dot(n, viewDir) >= 0.0f) {
		return fragment.varyings.frontColor;
	}
	return fragment.varyings.backColor;
}

}  // namespace hw1
