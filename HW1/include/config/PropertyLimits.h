#pragma once

namespace hw1 {

struct SliderSpec {
	float min;
	float max;
	float step;
};

namespace PropertyLimits {

inline constexpr SliderSpec ObjectScale{0.1f, 50.0f, 0.1f};
inline constexpr SliderSpec ObjectTranslation{-100.0f, 100.0f, 0.1f};
inline constexpr SliderSpec ObjectRotation{-360.0f, 360.0f, 1.0f};
inline constexpr SliderSpec ObjectShear{-2.0f, 2.0f, 0.01f};

inline constexpr SliderSpec CameraPosition{-100.0f, 100.0f, 0.1f};

inline constexpr SliderSpec LightPosition{-100.0f, 100.0f, 0.1f};
inline constexpr SliderSpec LightColor{0.0f, 1.0f, 0.01f};
inline constexpr SliderSpec LightIntensity{0.0f, 5.0f, 0.01f};
inline constexpr SliderSpec GlobalAmbient{0.0f, 1.0f, 0.01f};
inline constexpr SliderSpec GlobalSpecular{0.0f, 2.0f, 0.01f};
inline constexpr SliderSpec GlobalShininess{1.0f, 256.0f, 1.0f};

}  // namespace PropertyLimits

}  // namespace hw1
