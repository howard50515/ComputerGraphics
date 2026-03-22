#include "gui/WebServer.h"

#include <chrono>
#include <cstring>

namespace hw1 {

namespace {

void writeLe32(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint32_t value) {
	bytes[offset + 0] = static_cast<std::uint8_t>(value & 0xFFu);
	bytes[offset + 1] = static_cast<std::uint8_t>((value >> 8) & 0xFFu);
	bytes[offset + 2] = static_cast<std::uint8_t>((value >> 16) & 0xFFu);
	bytes[offset + 3] = static_cast<std::uint8_t>((value >> 24) & 0xFFu);
}

}  // namespace

void WebServer::encodeRgba(const Framebuffer& framebuffer, std::vector<std::uint8_t>& out) const {
	const auto encodeStart = std::chrono::steady_clock::now();
	const int width = framebuffer.width();
	const int height = framebuffer.height();
	if (width <= 0 || height <= 0) {
		latestEncodeMs_.store(0.0f);
		out.clear();
		return;
	}

	const std::size_t pixelBytes = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;
	out.resize(8u + pixelBytes);

	writeLe32(out, 0u, static_cast<std::uint32_t>(width));
	writeLe32(out, 4u, static_cast<std::uint32_t>(height));

	const auto& rgba = framebuffer.rgba8();
	if (rgba.size() == pixelBytes) {
		std::memcpy(out.data() + 8u, rgba.data(), pixelBytes);
	}

	const auto encodeEnd = std::chrono::steady_clock::now();
	latestEncodeMs_.store(std::chrono::duration<float, std::milli>(encodeEnd - encodeStart).count());

}

}  // namespace hw1
