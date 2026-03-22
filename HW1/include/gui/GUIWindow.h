#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "core/Color.h"
#include "core/Scene.h"
#include "core/SceneObject.h"
#include "gui/Menu.h"
#include "gui/Slider.h"
#include "material/Material.h"

namespace hw1 {

class GUIWindow {
public:
	enum class ShadingMode : int {
		Flat = 0,
		Gouraud = 1,
		Phong = 2,
		Unlit = 3
	};

	GUIWindow();

	void bindScene(Scene* scene);
	void bindPrimaryObject(const std::shared_ptr<SceneObject>& object);
	void bindShadingMaterials(
		const std::shared_ptr<Material>& flat,
		const std::shared_ptr<Material>& gouraud,
		const std::shared_ptr<Material>& phong,
		const std::shared_ptr<Material>& unlit);
	void applyRuntimeBindings();

	void setShadingMode(ShadingMode mode);
	ShadingMode shadingMode() const;
	void setActiveCameraIndex(std::size_t index);
	std::size_t activeCameraIndex() const;
	void setObjectScale(float uniformScale);
	float objectScale() const;

	std::vector<std::uint8_t> htmlPage() const;
	std::string shadingModeJson() const;
	std::string cameraJson() const;
	std::string objectJson() const;
	std::string lightsJson() const;
	bool trySetShadingModeFromRequest(const std::string& requestLine, std::string& outJsonPayload);
	bool trySetCameraFromRequest(const std::string& requestLine, std::string& outJsonPayload);
	bool trySetObjectScaleFromRequest(const std::string& requestLine, std::string& outJsonPayload);
	bool trySetCameraPositionFromRequest(const std::string& requestLine, std::string& outJsonPayload);
	bool trySetLightPositionFromRequest(const std::string& requestLine, std::string& outJsonPayload);
	bool trySetLightColorFromRequest(const std::string& requestLine, std::string& outJsonPayload);

	static const char* modeToString(ShadingMode mode);
	static bool tryParseMode(const std::string& text, ShadingMode& outMode);

private:
	static std::string extractModeValue(const std::string& requestLine);
	static std::string extractQueryValue(const std::string& requestLine, const std::string& prefix);

	Menu shadingMenu_;
	Menu cameraMenu_;
	std::atomic<int> shadingMode_;
	std::atomic<int> activeCameraIndex_;
	std::atomic<float> objectScale_;
	Scene* scene_;
	std::shared_ptr<SceneObject> primaryObject_;
	std::shared_ptr<Material> flatMaterial_;
	std::shared_ptr<Material> gouraudMaterial_;
	std::shared_ptr<Material> phongMaterial_;
	std::shared_ptr<Material> unlitMaterial_;
	ShadingMode lastAppliedShadingMode_;
	std::size_t lastAppliedCameraIndex_;
	float lastAppliedObjectScale_;

	struct PendingVector3Update {
		bool dirty;
		Vector3 value;
	};

	struct PendingColorUpdate {
		bool dirty;
		Color value;
	};

	mutable std::mutex pendingMutex_;
	std::vector<PendingVector3Update> pendingCameraPositions_;
	std::vector<PendingVector3Update> pendingLightPositions_;
	std::vector<PendingColorUpdate> pendingLightColors_;
};

}  // namespace hw1

