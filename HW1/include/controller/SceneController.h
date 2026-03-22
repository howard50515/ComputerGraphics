#pragma once

#include <cstddef>
#include <vector>

#include "core/Camera.h"
#include "core/Light.h"
#include "core/Mesh.h"
#include "core/Triangle.h"
#include "core/Color.h"
#include "data/VertexOutput.h"
#include "pipeline/Transform.h"
#include "pipeline/Clipper.h"
#include "pipeline/Rasterizer.h"

namespace hw1 {

/**
 * @class SceneController
 *
 * @brief Manages the scene, including meshes, cameras, lights,
 *        and the CPU-side rendering pipeline stages.
 *
 * Responsibilities:
 * - Store and manage meshes, cameras, and lights
 * - Apply transformations (scale, rotate, translate)
 * - Clip triangles outside the view frustum
 * - Compute lighting per vertex (Phong shading)
 * - Rasterize triangles into a framebuffer
 */
class SceneController {
public:

    // ----------------------------
    // Scene Setup Methods
    // ----------------------------

    void addMesh(const Mesh& mesh);
    void addCamera(const Camera& camera);
    void addLight(const Light& light);

    std::size_t meshCount() const;
    std::size_t cameraCount() const;
    std::size_t lightCount() const;

    const std::vector<Mesh>& meshes() const;
    const std::vector<Camera>& cameras() const;
    const std::vector<Light>& lights() const;

    // ----------------------------
    // Transformation Control
    // ----------------------------

    /**
     * @brief Set rotation angles for the model (degrees)
     */
    void setRotation(float x, float y, float z);

    /**
     * @brief Set uniform scaling factor for the model
     */
    void setScale(float s);

    /**
     * @brief Set translation vector for the model
     */
    void setTranslation(float x, float y, float z);

    // ----------------------------
    // Rendering Pipeline Methods
    // ----------------------------

    /**
     * @brief Apply model, view, and projection transformations
     *
     * Transforms object-space triangles to clip-space coordinates.
     *
     * @param tris Input triangles
     * @return Transformed triangles
     */
    std::vector<Triangle> applyTransform(const std::vector<Triangle>& tris) const;

    /**
     * @brief Clip triangles against the view frustum
     *
     * Removes or splits triangles that lie outside the visible volume.
     *
     * @param tris Input triangles
     * @return Clipped triangles
     */
    std::vector<Triangle> clipTriangles(const std::vector<Triangle>& tris) const;

    /**
     * @brief Compute per-vertex lighting (Phong shading)
     *
     * Uses the scene lights and vertex normals to compute colors.
     *
     * @param tris Input triangles
     * @return Triangles with shaded vertex colors
     */
    std::vector<Triangle> computeLighting(const std::vector<Triangle>& tris) const;

    /**
     * @brief Rasterize triangles into a framebuffer
     *
     * Converts triangles into pixels, performing color interpolation
     * and depth testing.
     *
     * @param tris Input triangles
     * @return Framebuffer (1D array of Color)
     */
    std::vector<Color> rasterize(const std::vector<Triangle>& tris) const;

private:

    // Scene objects
    std::vector<Mesh> meshes_;
    std::vector<Camera> cameras_;
    std::vector<Light> lights_;

    // Pipeline modules
    Transform transform_;
    Clipper clipper_;
    Rasterizer rasterizer_;

    // Transformation state
    float rotX_ = 0.0f;
    float rotY_ = 0.0f;
    float rotZ_ = 0.0f;
    float scale_ = 1.0f;
    Vector3 translation_ = {0.0f, 0.0f, 0.0f};
};

}  // namespace hw1
