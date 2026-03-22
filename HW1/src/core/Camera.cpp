#include "core/Camera.h"

#include <cmath>

#include "math/MathUtils.h"

namespace hw1 {

Camera::Camera()
	: position_(0.0f, 0.0f, 5.0f),
	  lookAt_(0.0f, 0.0f, 0.0f),
	  fovY_(60.0f),
	  aspect_(4.0f / 3.0f),
	  near_(0.1f),
	  far_(100.0f) {}

Camera::Camera(const Vector3& position, const Vector3& lookAtPoint)
	: position_(position),
	  lookAt_(lookAtPoint),
	  fovY_(60.0f),
	  aspect_(4.0f / 3.0f),
	  near_(0.1f),
	  far_(100.0f) {}

void Camera::setPosition(const Vector3& position) {
	position_ = position;
}

void Camera::setLookAt(const Vector3& lookAtPoint) {
	lookAt_ = lookAtPoint;
}

void Camera::setPerspective(float fovYDeg, float aspect, float nearPlane, float farPlane) {
	// Keep projection parameters valid to avoid NaNs and divide-by-zero.
	fovY_ = (fovYDeg > 1e-3f) ? fovYDeg : 1e-3f;
	aspect_ = (aspect > 1e-6f) ? aspect : 1.0f;
	near_ = (nearPlane > 1e-6f) ? nearPlane : 1e-3f;
	far_ = (farPlane > near_) ? farPlane : (near_ + 1.0f);
}

const Vector3& Camera::getPosition() const {
	return position_;
}

const Vector3& Camera::getLookAt() const {
	return lookAt_;
}

Matrix4x4 Camera::getViewMatrix() const {
	const Vector3 worldUp(0.0f, 1.0f, 0.0f);

	Vector3 forward = Normalize(lookAt_ - position_);
	if (forward.lengthSquared() <= 1e-8f) {
		forward = Vector3(0.0f, 0.0f, -1.0f);
	}

	Vector3 right = Normalize(Cross(forward, worldUp));
	if (right.lengthSquared() <= 1e-8f) {
		// Fallback when camera forward is nearly parallel to world up.
		right = Normalize(Cross(forward, Vector3(0.0f, 0.0f, 1.0f)));
	}

	Vector3 up = Cross(right, forward);

	Matrix4x4 view = Matrix4x4::identity();
	view.m[0][0] = right.x;
	view.m[0][1] = right.y;
	view.m[0][2] = right.z;
	view.m[0][3] = -Dot(right, position_);

	view.m[1][0] = up.x;
	view.m[1][1] = up.y;
	view.m[1][2] = up.z;
	view.m[1][3] = -Dot(up, position_);

	view.m[2][0] = -forward.x;
	view.m[2][1] = -forward.y;
	view.m[2][2] = -forward.z;
	view.m[2][3] = Dot(forward, position_);

	return view;
}

Matrix4x4 Camera::getProjectionMatrix() const {
	const float fovYRadians = fovY_ * (PI / 180.0f);
	const float f = 1.0f / std::tan(fovYRadians * 0.5f);

	Matrix4x4 projection;
	projection.m[0][0] = f / aspect_;
	projection.m[1][1] = f;
	projection.m[2][2] = (far_ + near_) / (near_ - far_);
	projection.m[2][3] = (2.0f * far_ * near_) / (near_ - far_);
	projection.m[3][2] = -1.0f;

	return projection;
}

}  // namespace hw1
