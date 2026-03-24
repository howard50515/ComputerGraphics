#pragma once

#include <string>

#include "config/LightingSettings.h"

namespace hw1 {

class LightSettingsPanel {
public:
	static std::string renderHtml(const LightingSettings& settings);
};

}  // namespace hw1
