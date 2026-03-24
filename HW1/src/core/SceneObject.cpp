#include "core/SceneObject.h"

#include "math/MathUtils.h"
#include "pipeline/Transform.h"

namespace hw1 {

SceneObject::SceneObject()
	: objectId_(kInvalidObjectId),
	  visible_(true),
	  rotationDeg_(0.0f, 0.0f, 0.0f),
	  scale_(1.0f, 1.0f, 1.0f),
	  translation_(0.0f, 0.0f, 0.0f),
	  shearXY_(0.0f),
	  shearXZ_(0.0f),
	  shearYX_(0.0f),
	  shearYZ_(0.0f),
	  shearZX_(0.0f),
	  shearZY_(0.0f),
	  modelMatrix_(Matrix4x4::identity()) {}

SceneObject::SceneObject(const Mesh& sourceMesh)
	: mesh_(sourceMesh),
	  objectId_(kInvalidObjectId),
	  visible_(true),
	  rotationDeg_(0.0f, 0.0f, 0.0f),
	  scale_(1.0f, 1.0f, 1.0f),
	  translation_(0.0f, 0.0f, 0.0f),
	  shearXY_(0.0f),
	  shearXZ_(0.0f),
	  shearYX_(0.0f),
	  shearYZ_(0.0f),
	  shearZX_(0.0f),
	  shearZY_(0.0f),
	  modelMatrix_(Matrix4x4::identity()) {}

void SceneObject::setObjectId(std::uint32_t id) {
	objectId_ = id;
}

std::uint32_t SceneObject::objectId() const {
	return objectId_;
}

void SceneObject::setVisible(bool visible) {
	visible_ = visible;
}

bool SceneObject::visible() const {
	return visible_;
}

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

void SceneObject::setShearXY(float value) {
	shearXY_ = value;
	updateModelMatrix();
}

void SceneObject::setShearXZ(float value) {
	shearXZ_ = value;
	updateModelMatrix();
}

void SceneObject::setShearYX(float value) {
	shearYX_ = value;
	updateModelMatrix();
}

void SceneObject::setShearYZ(float value) {
	shearYZ_ = value;
	updateModelMatrix();
}

void SceneObject::setShearZX(float value) {
	shearZX_ = value;
	updateModelMatrix();
}

void SceneObject::setShearZY(float value) {
	shearZY_ = value;
	updateModelMatrix();
}

void SceneObject::setMaterial(const std::shared_ptr<Material>& material) {
	material_ = material;
}

void SceneObject::setModelFile(const std::string& modelFile) {
	modelFile_ = modelFile;
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

const std::string& SceneObject::modelFile() const {
	return modelFile_;
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

float SceneObject::shearXY() const {
	return shearXY_;
}

float SceneObject::shearXZ() const {
	return shearXZ_;
}

float SceneObject::shearYX() const {
	return shearYX_;
}

float SceneObject::shearYZ() const {
	return shearYZ_;
}

float SceneObject::shearZX() const {
	return shearZX_;
}

float SceneObject::shearZY() const {
	return shearZY_;
}

void SceneObject::updateModelMatrix() {
	const float rx = rotationDeg_.x * (PI / 180.0f);
	const float ry = rotationDeg_.y * (PI / 180.0f);
	const float rz = rotationDeg_.z * (PI / 180.0f);

	const Matrix4x4 s = Transform::scale(scale_);
	const Matrix4x4 h = Transform::shear(shearXY_, shearXZ_, shearYX_, shearYZ_, shearZX_, shearZY_);
	const Matrix4x4 rX = Transform::rotationX(rx);
	const Matrix4x4 rY = Transform::rotationY(ry);
	const Matrix4x4 rZ = Transform::rotationZ(rz);
	const Matrix4x4 t = Transform::translation(translation_);

	modelMatrix_ = t * rZ * rY * rX * h * s;
}

}  // namespace hw1
