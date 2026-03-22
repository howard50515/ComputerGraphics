#include "core/SceneObject.h"

#include "math/MathUtils.h"
#include "pipeline/Transform.h"

namespace hw1 {

SceneObject::SceneObject()
	: rotationDeg_(0.0f, 0.0f, 0.0f),
	  scale_(1.0f, 1.0f, 1.0f),
	  translation_(0.0f, 0.0f, 0.0f),
	  modelMatrix_(Matrix4x4::identity()) {}

SceneObject::SceneObject(const Mesh& sourceMesh)
	: mesh_(sourceMesh),
	  rotationDeg_(0.0f, 0.0f, 0.0f),
	  scale_(1.0f, 1.0f, 1.0f),
	  translation_(0.0f, 0.0f, 0.0f),
	  modelMatrix_(Matrix4x4::identity()) {}

void SceneObject::setRotation(const Vector3& rotationDeg) {
	rotationDeg_ = rotationDeg;
	updateModelMatrix();
}

void SceneObject::setScale(const Vector3& scale) {
	scale_ = scale;
	updateModelMatrix();
}

void SceneObject::setTranslation(const Vector3& translation) {
	translation_ = translation;
	updateModelMatrix();
}

void SceneObject::setMaterial(const std::shared_ptr<Material>& material) {
	material_ = material;
}

const Mesh& SceneObject::mesh() const {
	return mesh_;
}

Mesh& SceneObject::mesh() {
	return mesh_;
}

const std::shared_ptr<Material>& SceneObject::material() const {
	return material_;
}

const Matrix4x4& SceneObject::modelMatrix() const {
	return modelMatrix_;
}

const Vector3& SceneObject::rotation() const {
	return rotationDeg_;
}

const Vector3& SceneObject::scale() const {
	return scale_;
}

const Vector3& SceneObject::translation() const {
	return translation_;
}

void SceneObject::updateModelMatrix() {
	const float rx = rotationDeg_.x * (PI / 180.0f);
	const float ry = rotationDeg_.y * (PI / 180.0f);
	const float rz = rotationDeg_.z * (PI / 180.0f);

	const Matrix4x4 s = Transform::scale(scale_);
	const Matrix4x4 rX = Transform::rotationX(rx);
	const Matrix4x4 rY = Transform::rotationY(ry);
	const Matrix4x4 rZ = Transform::rotationZ(rz);
	const Matrix4x4 t = Transform::translation(translation_);

	modelMatrix_ = t * rZ * rY * rX * s;
}

}  // namespace hw1
