#include "pipeline/Rasterizer.h"

#include <algorithm>
#include <cmath>

namespace hw1 {

namespace {

struct ScreenVertex {
	float x;
	float y;
	float z;
	Varyings varyings;
};

float EdgeFunction(float ax, float ay, float bx, float by, float px, float py) {
	return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

bool ToScreenVertex(const VertexOutput& in, int width, int height, ScreenVertex& out) {
	const float w = in.clipPosition.w;
	if (std::fabs(w) <= 1e-6f) {
		return false;
	}

	const float invW = 1.0f / w;
	const float ndcX = in.clipPosition.x * invW;
	const float ndcY = in.clipPosition.y * invW;
	const float ndcZ = in.clipPosition.z * invW;

	out.x = (ndcX * 0.5f + 0.5f) * static_cast<float>(width - 1);
	out.y = (1.0f - (ndcY * 0.5f + 0.5f)) * static_cast<float>(height - 1);
	out.z = ndcZ * 0.5f + 0.5f;
	out.varyings = in.varyings;
	return true;
}

Varyings InterpolateVaryings(const ScreenVertex& a, const ScreenVertex& b, const ScreenVertex& c, float w0, float w1, float w2) {
	Varyings out;
	out.normal = a.varyings.normal * w0 + b.varyings.normal * w1 + c.varyings.normal * w2;
	out.worldPosition = a.varyings.worldPosition * w0 + b.varyings.worldPosition * w1 + c.varyings.worldPosition * w2;
	out.uv = a.varyings.uv * w0 + b.varyings.uv * w1 + c.varyings.uv * w2;
	out.frontColor = Color(
		a.varyings.frontColor.r * w0 + b.varyings.frontColor.r * w1 + c.varyings.frontColor.r * w2,
		a.varyings.frontColor.g * w0 + b.varyings.frontColor.g * w1 + c.varyings.frontColor.g * w2,
		a.varyings.frontColor.b * w0 + b.varyings.frontColor.b * w1 + c.varyings.frontColor.b * w2);
	out.backColor = Color(
		a.varyings.backColor.r * w0 + b.varyings.backColor.r * w1 + c.varyings.backColor.r * w2,
		a.varyings.backColor.g * w0 + b.varyings.backColor.g * w1 + c.varyings.backColor.g * w2,
		a.varyings.backColor.b * w0 + b.varyings.backColor.b * w1 + c.varyings.backColor.b * w2);
	return out;
}

}  // namespace

Rasterizer::Rasterizer() : width_(0), height_(0) {}

void Rasterizer::setResolution(int width, int height) {
	width_ = width;
	height_ = height;
	framebuffer_.assign(static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_), 0x000000FFu);
}

void Rasterizer::clear(std::uint32_t rgba) {
	std::fill(framebuffer_.begin(), framebuffer_.end(), rgba);
}

void Rasterizer::drawMesh(const Mesh& mesh) {
	(void)mesh;
	if (!framebuffer_.empty()) {
		framebuffer_[0] = 0xFFFFFFFFu;
	}
}

void Rasterizer::rasterize(
	const std::array<VertexOutput, 3>& triangle,
	const Material& material,
	const UniformContext& ctx,
	hw1::Framebuffer& framebuffer) const {
	const int width = framebuffer.width();
	const int height = framebuffer.height();
	if (width <= 0 || height <= 0) {
		return;
	}

	ScreenVertex v0;
	ScreenVertex v1;
	ScreenVertex v2;
	if (!ToScreenVertex(triangle[0], width, height, v0) ||
		!ToScreenVertex(triangle[1], width, height, v1) ||
		!ToScreenVertex(triangle[2], width, height, v2)) {
		return;
	}

	const float area = EdgeFunction(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
	if (std::fabs(area) <= 1e-6f) {
		return;
	}

	const int minX = std::max(0, static_cast<int>(std::floor(std::min({v0.x, v1.x, v2.x}))));
	const int maxX = std::min(width - 1, static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x}))));
	const int minY = std::max(0, static_cast<int>(std::floor(std::min({v0.y, v1.y, v2.y}))));
	const int maxY = std::min(height - 1, static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y}))));

	for (int y = minY; y <= maxY; ++y) {
		const std::size_t rowOffset = static_cast<std::size_t>(y) * static_cast<std::size_t>(width);
		for (int x = minX; x <= maxX; ++x) {
			const float px = static_cast<float>(x) + 0.5f;
			const float py = static_cast<float>(y) + 0.5f;

			const float e0 = EdgeFunction(v1.x, v1.y, v2.x, v2.y, px, py);
			const float e1 = EdgeFunction(v2.x, v2.y, v0.x, v0.y, px, py);
			const float e2 = EdgeFunction(v0.x, v0.y, v1.x, v1.y, px, py);

			const bool hasNeg = (e0 < 0.0f) || (e1 < 0.0f) || (e2 < 0.0f);
			const bool hasPos = (e0 > 0.0f) || (e1 > 0.0f) || (e2 > 0.0f);
			if (hasNeg && hasPos) {
				continue;
			}

			const float w0 = e0 / area;
			const float w1 = e1 / area;
			const float w2 = e2 / area;

			const float depth = v0.z * w0 + v1.z * w1 + v2.z * w2;
			const std::size_t pixelIndex = rowOffset + static_cast<std::size_t>(x);
			if (!framebuffer.testAndSetDepthUnchecked(pixelIndex, depth)) {
				continue;
			}

			Fragment fragment;
			fragment.x = x;
			fragment.y = y;
			fragment.depth = depth;
			fragment.varyings = InterpolateVaryings(v0, v1, v2, w0, w1, w2);

			const Color shaded = material.fragmentShader(fragment, ctx);
			framebuffer.setPixelUnchecked(pixelIndex, shaded);
		}
	}
}

const std::vector<std::uint32_t>& Rasterizer::framebuffer() const {
	return framebuffer_;
}

}  // namespace hw1
