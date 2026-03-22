#include "core/Scene.h"

#include <stdexcept>

namespace hw1 {

Scene::Scene() : activeCameraIndex_(0) {}

void Scene::addObject(const std::shared_ptr<SceneObject>& obj) {
	if (obj) {
		objects_.push_back(obj);
	}
}

void Scene::addObject(const SceneObject& obj) {
	objects_.push_back(std::make_shared<SceneObject>(obj));
}

void Scene::addCamera(const std::shared_ptr<Camera>& camera) {
	if (camera) {
		cameras_.push_back(camera);
		activeCameraIndex_ = cameras_.size() - 1;
	}
}

void Scene::addCamera(const Camera& camera) {
	cameras_.push_back(std::make_shared<Camera>(camera));
	activeCameraIndex_ = cameras_.size() - 1;
}

void Scene::addLight(const std::shared_ptr<Light>& light) {
	if (light) {
		lights_.push_back(light);
	}
}

void Scene::addLight(const Light& light) {
	lights_.push_back(std::make_shared<Light>(light));
}

const std::vector<std::shared_ptr<SceneObject>>& Scene::objects() const {
	return objects_;
}

const std::vector<std::shared_ptr<Camera>>& Scene::cameras() const {
	return cameras_;
}

const std::vector<std::shared_ptr<Light>>& Scene::lights() const {
	return lights_;
}

void Scene::setActiveCameraIndex(std::size_t index) {
	if (index >= cameras_.size()) {
		throw std::out_of_range("Active camera index is out of range.");
	}
	if (!cameras_[index]) {
		throw std::runtime_error("Target active camera is null.");
	}
	activeCameraIndex_ = index;
}

std::size_t Scene::activeCameraIndex() const {
	return activeCameraIndex_;
}

Camera& Scene::activeCamera() {
	if (cameras_.empty()) {
		throw std::runtime_error("Scene has no camera.");
	}
	if (!cameras_[activeCameraIndex_]) {
		throw std::runtime_error("Active camera is null.");
	}
	return *cameras_[activeCameraIndex_];
}

const Camera& Scene::activeCamera() const {
	if (cameras_.empty()) {
		throw std::runtime_error("Scene has no camera.");
	}
	if (!cameras_[activeCameraIndex_]) {
		throw std::runtime_error("Active camera is null.");
	}
	return *cameras_[activeCameraIndex_];
}

}  // namespace hw1
