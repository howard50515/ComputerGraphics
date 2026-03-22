#include "pipeline/Renderer.h"

#include <array>
#include <chrono>
#include <stdexcept>

#include "material/Material.h"
#include "math/MathUtils.h"

namespace hw1 {

namespace {

bool IsTriangleFacingCamera(const std::array<VertexOutput, 3>& triangle, const Vector3& cameraPosition) {
	const Vector3 p0 = triangle[0].varyings.worldPosition;
	const Vector3 p1 = triangle[1].varyings.worldPosition;
	const Vector3 p2 = triangle[2].varyings.worldPosition;

	Vector3 normal = Cross(p1 - p0, p2 - p0);
	if (normal.lengthSquared() <= 1e-12f) {
		normal = triangle[0].varyings.normal + triangle[1].varyings.normal + triangle[2].varyings.normal;
		if (normal.lengthSquared() <= 1e-12f) {
			return false;
		}
	}
	normal = Normalize(normal);

	const Vector3 center = (p0 + p1 + p2) / 3.0f;
	Vector3 viewDir = cameraPosition - center;
	if (viewDir.lengthSquared() <= 1e-12f) {
		return false;
	}
	viewDir = Normalize(viewDir);

	return Dot(normal, viewDir) > 0.0f;
}

}  // namespace

Renderer::Renderer(int width, int height)
	: framebuffer_(width, height),
	  lastGeometryMs_(0.0f),
	  lastRasterMs_(0.0f) {}

void Renderer::resize(int width, int height) {
	framebuffer_.resize(width, height);
}

int Renderer::width() const {
	return framebuffer_.width();
}

int Renderer::height() const {
	return framebuffer_.height();
}

float Renderer::aspectRatio() const {
	const int h = framebuffer_.height();
	if (h <= 0) {
		return 1.0f;
	}
	return static_cast<float>(framebuffer_.width()) / static_cast<float>(h);
}

float Renderer::geometryMs() const {
	return lastGeometryMs_;
}

float Renderer::rasterMs() const {
	return lastRasterMs_;
}

const Framebuffer& Renderer::render(const Scene& scene) {
	using Clock = std::chrono::steady_clock;
	float geometryMsAccum = 0.0f;
	float rasterMsAccum = 0.0f;

	framebuffer_.clear(Color(0.0f, 0.0f, 0.0f));

	UniformContext ctx;
	ctx.scene.lights.clear();
	ctx.scene.lights.reserve(scene.lights().size());
	for (const auto& light : scene.lights()) {
		if (light) {
			ctx.scene.lights.push_back(*light);
		}
	}
	ctx.scene.cameraPosition = scene.activeCamera().getPosition();
	ctx.transform.view = scene.activeCamera().getViewMatrix();
	ctx.transform.projection = scene.activeCamera().getProjectionMatrix();

	for (const auto& object : scene.objects()) {
		if (!object || !object->material()) {
			continue;
		}
		Material* material = object->material().get();

		ctx.transform.model = object->modelMatrix();
		ctx.transform.MVP = ctx.transform.projection * ctx.transform.view * ctx.transform.model;

		for (const Triangle& tri : object->mesh().getTriangles()) {
			const auto geometryStart = Clock::now();
			std::array<Vertex, 3> inputVertices = {tri.v0, tri.v1, tri.v2};
			std::array<VertexOutput, 3> v;

			for (std::size_t i = 0u; i < 3u; ++i) {
				v[i] = material->vertexShader(inputVertices[i], tri.frontColor, tri.backColor, ctx);
			}

			if (!IsTriangleFacingCamera(v, ctx.scene.cameraPosition)) {
				const auto geometryEnd = Clock::now();
				geometryMsAccum += std::chrono::duration<float, std::milli>(geometryEnd - geometryStart).count();
				continue;
			}

			material->prepareTriangle(v, ctx);

			const auto clipped = clipper_.clip(v);
			const auto geometryEnd = Clock::now();
			geometryMsAccum += std::chrono::duration<float, std::milli>(geometryEnd - geometryStart).count();
			for (const auto& clippedTri : clipped) {
				const auto rasterStart = Clock::now();
				rasterizer_.rasterize(clippedTri, *material, ctx, framebuffer_);
				const auto rasterEnd = Clock::now();
				rasterMsAccum += std::chrono::duration<float, std::milli>(rasterEnd - rasterStart).count();
			}
		}
	}

	lastGeometryMs_ = geometryMsAccum;
	lastRasterMs_ = rasterMsAccum;

	return framebuffer_;
}

}  // namespace hw1
