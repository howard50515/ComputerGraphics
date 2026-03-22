#include "core/Framebuffer.h"

#include <algorithm>
#include <limits>

namespace hw1 {

namespace {

std::uint8_t toByte(float v) {
	float clamped = v;
	if (clamped < 0.0f) {
		clamped = 0.0f;
	}
	if (clamped > 1.0f) {
		clamped = 1.0f;
	}
	return static_cast<std::uint8_t>(clamped * 255.0f + 0.5f);
}

void writeRgbaAt(std::vector<std::uint8_t>& rgba, std::size_t idx, const Color& color) {
	const std::size_t base = idx * 4u;
	rgba[base + 0] = toByte(color.r);
	rgba[base + 1] = toByte(color.g);
	rgba[base + 2] = toByte(color.b);
	rgba[base + 3] = 255u;
}

}  // namespace

Framebuffer::Framebuffer() : width_(0), height_(0) {}

Framebuffer::Framebuffer(int width, int height) : width_(0), height_(0) {
	resize(width, height);
}

void Framebuffer::resize(int width, int height) {
	width_ = width;
	height_ = height;
	const std::size_t size = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
	pixels_.assign(size, Color(0.0f, 0.0f, 0.0f));
	depth_.assign(size, std::numeric_limits<float>::infinity());
	rgba8_.assign(size * 4u, 0u);
	for (std::size_t i = 0; i < size; ++i) {
		rgba8_[i * 4u + 3u] = 255u;
	}
}

void Framebuffer::clear(const Color& color) {
	std::fill(pixels_.begin(), pixels_.end(), color);
	std::fill(depth_.begin(), depth_.end(), std::numeric_limits<float>::infinity());
	const std::uint8_t r = toByte(color.r);
	const std::uint8_t g = toByte(color.g);
	const std::uint8_t b = toByte(color.b);
	const std::size_t pixelCount = pixels_.size();
	for (std::size_t i = 0; i < pixelCount; ++i) {
		const std::size_t base = i * 4u;
		rgba8_[base + 0u] = r;
		rgba8_[base + 1u] = g;
		rgba8_[base + 2u] = b;
		rgba8_[base + 3u] = 255u;
	}
}

void Framebuffer::setPixel(int x, int y, const Color& color) {
	if (x < 0 || y < 0 || x >= width_ || y >= height_) {
		return;
	}
	const std::size_t idx = indexOf(x, y);
	pixels_[idx] = color;
	writeRgbaAt(rgba8_, idx, color);
}

bool Framebuffer::setPixelDepthTest(int x, int y, float depth, const Color& color) {
	if (x < 0 || y < 0 || x >= width_ || y >= height_) {
		return false;
	}

	const std::size_t idx = indexOf(x, y);
	return setPixelDepthTestUnchecked(idx, depth, color);
}

bool Framebuffer::setPixelDepthTestUnchecked(std::size_t idx, float depth, const Color& color) {
	if (depth < depth_[idx]) {
		depth_[idx] = depth;
		pixels_[idx] = color;
		writeRgbaAt(rgba8_, idx, color);
		return true;
	}

	return false;
}

bool Framebuffer::testAndSetDepthUnchecked(std::size_t idx, float depth) {
	if (depth < depth_[idx]) {
		depth_[idx] = depth;
		return true;
	}

	return false;
}

void Framebuffer::setPixelUnchecked(std::size_t idx, const Color& color) {
	pixels_[idx] = color;
	writeRgbaAt(rgba8_, idx, color);
}

int Framebuffer::width() const {
	return width_;
}

int Framebuffer::height() const {
	return height_;
}

const std::vector<Color>& Framebuffer::pixels() const {
	return pixels_;
}

const std::vector<float>& Framebuffer::depthBuffer() const {
	return depth_;
}

const std::vector<std::uint8_t>& Framebuffer::rgba8() const {
	return rgba8_;
}

std::size_t Framebuffer::indexOf(int x, int y) const {
	return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x);
}

}  // namespace hw1
