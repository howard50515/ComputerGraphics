#include <iostream>
#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <thread>

#include "core/Camera.h"
#include "core/Light.h"
#include "core/Scene.h"
#include "gui/WebServer.h"
#include "io/MeshLoader.h"
#include "material/FlatMaterial.h"
#include "material/GouraudMaterial.h"
#include "material/PhongMaterial.h"
#include "material/UnlitMaterial.h"
#include "pipeline/Renderer.h"

int main() {
	using namespace hw1;

	try {
		Scene scene;

		Mesh meshTeapot = MeshLoader::loadFromFile("examples/Teapot.json");
		Mesh meshKangaroo = MeshLoader::loadFromFile("examples/Kangaroo.json");
		Mesh meshFighter = MeshLoader::loadFromFile("examples/Csie.json");

		auto obj = std::make_shared<SceneObject>(meshTeapot);
		auto obj2 = std::make_shared<SceneObject>(meshKangaroo);
		auto obj3 = std::make_shared<SceneObject>(meshFighter);
		const auto flatMaterial = std::make_shared<FlatMaterial>();
		const auto gouraudMaterial = std::make_shared<GouraudMaterial>();
		const auto phongMaterial = std::make_shared<PhongMaterial>();
		const auto unlitMaterial = std::make_shared<UnlitMaterial>();
		obj->setMaterial(gouraudMaterial);
		obj2->setMaterial(gouraudMaterial);
		obj3->setMaterial(gouraudMaterial);
		obj->setModelFile("Teapot.json");
		obj2->setModelFile("Kangaroo.json");
		obj3->setModelFile("Csie.json");
		scene.addObject(obj);
		scene.addObject(obj2);
		scene.addObject(obj3);

		obj->setRotation(Vector3(0.0f, 0.0f, 0.0f));
		obj->setScale(Vector3(1.0f, 1.0f, 1.0f));
		obj->setTranslation(Vector3(-40.0f, 0.0f, 0.0f));
		obj2->setRotation(Vector3(0.0f, 0.0f, 0.0f));
		obj2->setScale(Vector3(40.0f, 40.0f, 40.0f));
		obj2->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
		obj3->setRotation(Vector3(0.0f, 0.0f, 0.0f));
		obj3->setScale(Vector3(20.0f, 20.0f, 20.0f));
		obj3->setTranslation(Vector3(20.0f, 0.0f, 0.0f));

		auto objectCam = std::make_shared<Camera>(Vector3(0.0f, 0.0f, 80.0f), Vector3(0.0f, 0.0f, 0.0f));
		auto sideCam = std::make_shared<Camera>(Vector3(30.0f, 3.0f, 30.0f), Vector3(0.0f, 0.0f, 0.0f));
		scene.addCamera(objectCam);
		scene.addCamera(sideCam);

		auto keyLight = std::make_shared<Light>(Vector3(50.0f, 50.0f, 50.0f), Color(1.0f, 1.0f, 1.0f), 1.0f);
		auto fillLight = std::make_shared<Light>(Vector3(-50.0f, -50.0f, 25.0f), Color(0.0f, 1.0f, 1.0f), 1.0f);
		scene.addLight(keyLight);
		scene.addLight(fillLight);

		scene.setActiveCameraIndex(0);

		Renderer renderer(800, 600);
		objectCam->setPerspective(60.0f, renderer.aspectRatio(), 0.1f, 100.0f);
		sideCam->setPerspective(60.0f, renderer.aspectRatio(), 0.1f, 100.0f);

		WebServer webServer;
		webServer.bindScene(&scene);
		webServer.bindShadingMaterials(flatMaterial, gouraudMaterial, phongMaterial, unlitMaterial);
		webServer.guiWindow().setActiveCameraIndex(scene.activeCameraIndex());
		webServer.start(8080);
		std::cout << "Live framebuffer viewer: http://127.0.0.1:8080\n";
		std::cout << "Press Ctrl+C to stop.\n";
		webServer.guiWindow().applyRuntimeBindings();

		scene.setActiveCameraIndex(0);
		std::uint32_t frameCount = 0;
		auto fpsWindowStart = std::chrono::steady_clock::now();

		while (!webServer.shutdownRequested()) {
			webServer.guiWindow().applyRuntimeBindings();

			const auto renderStart = std::chrono::steady_clock::now();
			const Framebuffer& framebuffer = renderer.render(scene);
			const auto renderEnd = std::chrono::steady_clock::now();
			const float renderMs = std::chrono::duration<float, std::milli>(renderEnd - renderStart).count();
			webServer.setRenderMs(renderMs);
			webServer.setGeometryMs(renderer.geometryMs());
			webServer.setRasterMs(renderer.rasterMs());

			const auto uploadStart = std::chrono::steady_clock::now();
			webServer.setFrame(framebuffer);
			const auto uploadEnd = std::chrono::steady_clock::now();
			const float uploadMs = std::chrono::duration<float, std::milli>(uploadEnd - uploadStart).count();
			webServer.setUploadMs(uploadMs);

			++frameCount;
			const auto now = std::chrono::steady_clock::now();
			const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - fpsWindowStart).count();
			if (elapsedMs >= 1000) {
				const float fps = static_cast<float>(frameCount) * 1000.0f / static_cast<float>(elapsedMs);
				webServer.setFps(fps);
				frameCount = 0;
				fpsWindowStart = now;
			}

			// std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		webServer.stop();
		std::cout << "Renderer stopped by web request.\n";
	} catch (const std::exception& ex) {
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}
