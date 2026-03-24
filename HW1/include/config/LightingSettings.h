#pragma once

namespace hw1 {

struct LightingSettings {
	float ambientStrength;
	float specularStrength;
	float shininess;
};

// Global lighting tuning shared by all lit materials.
LightingSettings& GlobalLightingSettings();
const LightingSettings& GlobalLightingSettingsConst();

}  // namespace hw1
