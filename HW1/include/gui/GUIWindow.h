#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/Color.h"
#include "core/Scene.h"
#include "material/Material.h"

namespace hw1 {

class GUIWindow {
public:
	GUIWindow();

	void bindScene(Scene* scene);
	void bindShadingMaterials(
		const std::shared_ptr<Material>& flat,
		const std::shared_ptr<Material>& gouraud,
		const std::shared_ptr<Material>& phong,
		const std::shared_ptr<Material>& unlit);
	void applyRuntimeBindings();

	void setActiveCameraIndex(std::size_t index);
	std::size_t activeCameraIndex() const;

	std::vector<std::uint8_t> htmlPage() const;
	std::string shadingModeJson() const;
	std::string cameraJson() const;
	std::string objectJson() const;
	std::string lightsJson() const;

private:
	std::string selectedMaterialName(const std::shared_ptr<SceneObject>& object) const;

	std::atomic<int> activeCameraIndex_;
	Scene* scene_;
	std::shared_ptr<Material> flatMaterial_;
	std::shared_ptr<Material> gouraudMaterial_;
	std::shared_ptr<Material> phongMaterial_;
	std::shared_ptr<Material> unlitMaterial_;
	std::size_t lastAppliedCameraIndex_;
};

}  // namespace hw1

