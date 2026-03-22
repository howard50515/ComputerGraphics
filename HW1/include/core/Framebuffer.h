#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/Color.h"

namespace hw1 {

class Framebuffer {
public:
	Framebuffer();
	Framebuffer(int width, int height);

	void resize(int width, int height);
	void clear(const Color& color = Color(0.0f, 0.0f, 0.0f));
	void setPixel(int x, int y, const Color& color);
	bool setPixelDepthTest(int x, int y, float depth, const Color& color);
	bool setPixelDepthTestUnchecked(std::size_t index, float depth, const Color& color);
	bool testAndSetDepthUnchecked(std::size_t index, float depth);
	void setPixelUnchecked(std::size_t index, const Color& color);

	int width() const;
	int height() const;
	const std::vector<Color>& pixels() const;
	const std::vector<float>& depthBuffer() const;
	const std::vector<std::uint8_t>& rgba8() const;

private:
	std::size_t indexOf(int x, int y) const;

	int width_;
	int height_;
	std::vector<Color> pixels_;
	std::vector<float> depth_;
	std::vector<std::uint8_t> rgba8_;
};

}  // namespace hw1
