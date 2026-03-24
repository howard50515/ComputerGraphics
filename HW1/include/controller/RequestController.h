#pragma once

#include <memory>
#include <string>

#include "core/Scene.h"
#include "material/Material.h"

namespace hw1 {

struct ControllerResult {
	int statusCode;
	std::string contentType;
	std::string payload;
};

class RequestController {
public:
	RequestController();

	void bindScene(Scene* scene);
	void bindShadingMaterials(
		const std::shared_ptr<Material>& flat,
		const std::shared_ptr<Material>& gouraud,
		const std::shared_ptr<Material>& phong,
		const std::shared_ptr<Material>& unlit);

	bool handleSetShading(const std::string& requestLine, ControllerResult& out) const;
	bool handleSetLightingSettings(const std::string& requestLine, ControllerResult& out) const;
	bool handleSetCamera(const std::string& requestLine, ControllerResult& out) const;
	bool handleSetCameraPosition(const std::string& requestLine, ControllerResult& out) const;
	bool handleSetLight(const std::string& requestLine, ControllerResult& out) const;
	bool handleSetObjectProperty(const std::string& requestLine, ControllerResult& out) const;

private:
	Scene* scene_;
	std::shared_ptr<Material> flatMaterial_;
	std::shared_ptr<Material> gouraudMaterial_;
	std::shared_ptr<Material> phongMaterial_;
	std::shared_ptr<Material> unlitMaterial_;
};

}  // namespace hw1
