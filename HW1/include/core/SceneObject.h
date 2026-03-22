#pragma once

#include <memory>

#include "core/Mesh.h"
#include "material/Material.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"

namespace hw1 {

class SceneObject {
public:
	SceneObject();
	explicit SceneObject(const Mesh& sourceMesh);

	void setRotation(const Vector3& rotationDeg);
	void setScale(const Vector3& scale);
	void setTranslation(const Vector3& translation);
	void setMaterial(const std::shared_ptr<Material>& material);

	const Mesh& mesh() const;
	Mesh& mesh();
	const std::shared_ptr<Material>& material() const;
	const Matrix4x4& modelMatrix() const;

	const Vector3& rotation() const;
	const Vector3& scale() const;
	const Vector3& translation() const;

private:
	void updateModelMatrix();

	Mesh mesh_;
	std::shared_ptr<Material> material_;
	Vector3 rotationDeg_;
	Vector3 scale_;
	Vector3 translation_;
	Matrix4x4 modelMatrix_;
};

}  // namespace hw1
