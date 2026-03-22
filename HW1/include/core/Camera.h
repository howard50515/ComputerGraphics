#pragma once

#include "math/Matrix4x4.h"
#include "math/Vector3.h"

namespace hw1 {

class Camera {
public:
	Camera();
	Camera(const Vector3& position, const Vector3& lookAtPoint);

	void setPosition(const Vector3& position);
	void setLookAt(const Vector3& lookAtPoint);
	void setPerspective(float fovYDeg, float aspect, float nearPlane, float farPlane);

	const Vector3& getPosition() const;
	const Vector3& getLookAt() const;

	Matrix4x4 getViewMatrix() const;
	Matrix4x4 getProjectionMatrix() const;

private:
	Vector3 position_;
	Vector3 lookAt_;
	float fovY_;
	float aspect_;
	float near_;
	float far_;
};

}  // namespace hw1
