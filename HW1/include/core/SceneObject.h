#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "core/Mesh.h"
#include "material/Material.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"

namespace hw1 {

class SceneObject {
public:
	static constexpr std::uint32_t kInvalidObjectId = static_cast<std::uint32_t>(-1);

	SceneObject();
	explicit SceneObject(const Mesh& sourceMesh);

	void setObjectId(std::uint32_t id);
	std::uint32_t objectId() const;
	void setVisible(bool visible);
	bool visible() const;

	void setRotation(const Vector3& rotationDeg);
	void setScale(const Vector3& scale);
	void setTranslation(const Vector3& translation);
	void setShearXY(float value);
	void setShearXZ(float value);
	void setShearYX(float value);
	void setShearYZ(float value);
	void setShearZX(float value);
	void setShearZY(float value);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setModelFile(const std::string& modelFile);

	const Mesh& mesh() const;
	Mesh& mesh();
	const std::shared_ptr<Material>& material() const;
	const std::string& modelFile() const;
	const Matrix4x4& modelMatrix() const;

	const Vector3& rotation() const;
	const Vector3& scale() const;
	const Vector3& translation() const;
	float shearXY() const;
	float shearXZ() const;
	float shearYX() const;
	float shearYZ() const;
	float shearZX() const;
	float shearZY() const;

private:
	void updateModelMatrix();

	Mesh mesh_;
	std::shared_ptr<Material> material_;
	std::string modelFile_;
	std::uint32_t objectId_;
	bool visible_;
	Vector3 rotationDeg_;
	Vector3 scale_;
	Vector3 translation_;
	float shearXY_;
	float shearXZ_;
	float shearYX_;
	float shearYZ_;
	float shearZX_;
	float shearZY_;
	Matrix4x4 modelMatrix_;
};

}  // namespace hw1
