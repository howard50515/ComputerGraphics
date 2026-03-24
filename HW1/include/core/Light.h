#pragma once

// Represents a light source in the scene.
// Stores position, color, and intensity used during lighting calculations.
#include "math/Vector3.h"
#include "core/Color.h"

namespace hw1 {

struct Light {
	Vector3 position;
	Color color;
	float intensity;
	bool visible;

	Light()
		: position(0.0f, 0.0f, 0.0f),
		  color(1.0f, 1.0f, 1.0f),
		  intensity(1.0f),
		  visible(true) {}

	Light(const Vector3& p, const Color& c, float i)
		: position(p),
		  color(c),
		  intensity(i),
		  visible(true) {}
};

}  // namespace hw1