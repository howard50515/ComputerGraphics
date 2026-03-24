#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "controller/RequestController.h"
#include "core/Framebuffer.h"
#include "gui/GUIWindow.h"
#include "material/Material.h"
#include "core/Scene.h"

namespace hw1 {

class WebServer {
public:
	WebServer();
	~WebServer();

	void start(std::uint16_t port);
	void stop();

	void setFrame(const Framebuffer& framebuffer);
	void setFps(float fps);
	void setRenderMs(float renderMs);
	void setGeometryMs(float geometryMs);
	void setRasterMs(float rasterMs);
	void setUploadMs(float uploadMs);
	void bindScene(Scene* scene);
	void bindShadingMaterials(
		const std::shared_ptr<Material>& flat,
		const std::shared_ptr<Material>& gouraud,
		const std::shared_ptr<Material>& phong,
		const std::shared_ptr<Material>& unlit);
	GUIWindow& guiWindow();
	const GUIWindow& guiWindow() const;
	bool shutdownRequested() const;

private:
	void serverLoop();
	void handleClient(std::uintptr_t clientSocket);

	void encodeRgba(const Framebuffer& framebuffer, std::vector<std::uint8_t>& out) const;
	std::atomic<bool> running_;
	std::uint16_t port_;
	std::thread serverThread_;

	std::mutex frameMutex_;
	std::vector<std::uint8_t> latestFrameRgba_;
	std::atomic<float> latestFps_;
	std::atomic<float> latestRenderMs_;
	std::atomic<float> latestGeometryMs_;
	std::atomic<float> latestRasterMs_;
	std::atomic<float> latestUploadMs_;
	mutable std::atomic<float> latestEncodeMs_;
	RequestController controller_;
	GUIWindow window_;
	std::atomic<bool> shutdownRequested_;
};

}  // namespace hw1
