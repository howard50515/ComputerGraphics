#include "controller/SceneController.h"

#include <cmath>

#include "math/MathUtils.h"

namespace hw1 {

void SceneController::addMesh(const Mesh& mesh) {
	meshes_.push_back(mesh);
}

void SceneController::addCamera(const Camera& camera) {
	cameras_.push_back(camera);
}

void SceneController::addLight(const Light& light) {
	lights_.push_back(light);
}

std::size_t SceneController::meshCount() const {
	return meshes_.size();
}

std::size_t SceneController::cameraCount() const {
	return cameras_.size();
}

std::size_t SceneController::lightCount() const {
	return lights_.size();
}

const std::vector<Mesh>& SceneController::meshes() const {
	return meshes_;
}

const std::vector<Camera>& SceneController::cameras() const {
	return cameras_;
}

const std::vector<Light>& SceneController::lights() const {
	return lights_;
}

void SceneController::setRotation(float x, float y, float z) {
	rotX_ = x;
	rotY_ = y;
	rotZ_ = z;
}

void SceneController::setScale(float s) {
	scale_ = s;
}

void SceneController::setTranslation(float x, float y, float z) {
	translation_ = Vector3(x, y, z);
}

std::vector<Triangle> SceneController::applyTransform(const std::vector<Triangle>& tris) const {
	const float radiansY = rotY_ * (PI / 180.0f);
	const Matrix4x4 scale = Transform::scale(Vector3(scale_, scale_, scale_));
	const Matrix4x4 rotateY = Transform::rotationY(radiansY);
	const Matrix4x4 translate = Transform::translation(translation_);
	const Matrix4x4 model = translate * rotateY * scale;

	std::vector<Triangle> out = tris;
	for (Triangle& tri : out) {
		auto applyToVertex = [&](Vertex& v) {
			const Vector4 p(v.position.x, v.position.y, v.position.z, 1.0f);
			const Vector4 transformed = model * p;
			v.position = Vector3(transformed.x, transformed.y, transformed.z);
		};

		applyToVertex(tri.v0);
		applyToVertex(tri.v1);
		applyToVertex(tri.v2);
	}

	(void)rotX_;
	(void)rotZ_;
	return out;
}

std::vector<Triangle> SceneController::clipTriangles(const std::vector<Triangle>& tris) const {
	return tris;
}

std::vector<Triangle> SceneController::computeLighting(const std::vector<Triangle>& tris) const {
	return tris;
}

std::vector<Color> SceneController::rasterize(const std::vector<Triangle>& tris) const {
	Mesh mesh;
	for (const Triangle& tri : tris) {
		mesh.addTriangle(tri);
	}

	Rasterizer rasterizer = rasterizer_;
	rasterizer.setResolution(400, 400);
	rasterizer.clear(0x000000FFu);
	rasterizer.drawMesh(mesh);

	const std::vector<std::uint32_t>& pixels = rasterizer.framebuffer();
	std::vector<Color> out;
	out.reserve(pixels.size());

	for (std::uint32_t rgba : pixels) {
		const float r = static_cast<float>((rgba >> 24) & 0xFFu) / 255.0f;
		const float g = static_cast<float>((rgba >> 16) & 0xFFu) / 255.0f;
		const float b = static_cast<float>((rgba >> 8) & 0xFFu) / 255.0f;
		out.emplace_back(r, g, b);
	}

	return out;
}

}  // namespace hw1
