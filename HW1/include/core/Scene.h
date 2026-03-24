#pragma once

// Stores the entire scene including all objects, lights, and the active camera.
// Acts as the root container that the renderer reads from during rendering.
#include <cstdint>
#include <memory>
#include <vector>

#include "core/Camera.h"
#include "core/Light.h"
#include "core/SceneObject.h"

namespace hw1 {

class Scene {
public:
    Scene();

    void addObject(const std::shared_ptr<SceneObject>& obj);
    void addObject(const SceneObject& obj);

    void addCamera(const std::shared_ptr<Camera>& camera);
    void addCamera(const Camera& camera);

    void addLight(const std::shared_ptr<Light>& light);
    void addLight(const Light& light);

    const std::vector<std::shared_ptr<SceneObject>>& objects() const;
    std::size_t objectCount() const;
    std::shared_ptr<SceneObject> getObjectById(std::uint32_t id) const;
    const std::vector<std::shared_ptr<Camera>>& cameras() const;
    const std::vector<std::shared_ptr<Light>>& lights() const;

    void setActiveCameraIndex(std::size_t index);
    std::size_t activeCameraIndex() const;

    Camera& activeCamera();
    const Camera& activeCamera() const;

private:
    std::vector<std::shared_ptr<SceneObject>> objects_;
    std::uint32_t nextObjectId_;
    std::vector<std::shared_ptr<Camera>> cameras_;
    std::vector<std::shared_ptr<Light>> lights_;
    std::size_t activeCameraIndex_;
};

} // namespace hw1
