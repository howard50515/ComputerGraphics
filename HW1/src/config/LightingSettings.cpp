#include "config/LightingSettings.h"

namespace hw1 {

namespace {

LightingSettings gLightingSettings{0.15f, 0.35f, 32.0f};

}  // namespace

LightingSettings& GlobalLightingSettings() {
	return gLightingSettings;
}

const LightingSettings& GlobalLightingSettingsConst() {
	return gLightingSettings;
}

}  // namespace hw1
