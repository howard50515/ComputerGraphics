#pragma once

// Main rendering engine that executes the full rendering pipeline.
// Iterates through scene objects and runs vertex processing,
// clipping, rasterization, and fragment shading.

#include "core/Framebuffer.h"
#include "core/Scene.h"
#include "pipeline/Clipper.h"
#include "pipeline/Rasterizer.h"

namespace hw1 {

class Renderer {
public:
	Renderer(int width, int height);

	void resize(int width, int height);
	int width() const;
	int height() const;
	float aspectRatio() const;
	float geometryMs() const;
	float rasterMs() const;

	const Framebuffer& render(const Scene& scene);

private:
	Framebuffer framebuffer_;
	Clipper clipper_;
	Rasterizer rasterizer_;
	float lastGeometryMs_;
	float lastRasterMs_;
};

}  // namespace hw1
