#pragma once

#include <string>

#include <vector>

#include "core/SceneObject.h"

namespace hw1 {

class ObjectControlPanel {
public:
	static std::string renderHtml(
		const SceneObject& object,
		const std::string& selectedMaterial,
		const std::vector<std::string>& modelOptions,
		const std::string& selectedModel);
};

}  // namespace hw1
