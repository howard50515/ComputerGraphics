#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "core/Color.h"
#include "core/Framebuffer.h"
#include "data/Fragment.h"
#include "data/VertexOutput.h"
#include "core/Mesh.h"
#include "material/Material.h"
#include "uniform/UniformContext.h"

namespace hw1 {

class Rasterizer {
public:
	Rasterizer();

	void setResolution(int width, int height);
	void clear(std::uint32_t rgba);
	void drawMesh(const Mesh& mesh);

	void rasterize(
		const std::array<VertexOutput, 3>& triangle,
		const Material& material,
		const UniformContext& ctx,
		hw1::Framebuffer& framebuffer) const;

	const std::vector<std::uint32_t>& framebuffer() const;

private:
	int width_;
	int height_;
	std::vector<std::uint32_t> framebuffer_;
};

}  // namespace hw1
