#include "core/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace engine
{
    Camera::Camera(const Viewport &viewport) :
        viewport(viewport),
        transform(1.0),
        position(0.0)
    {

    }

    const glm::mat4 &Camera::getTransform() const { return this->transform; }

    void Camera::setViewport(const Viewport &viewport)
    {
        this->viewport = viewport;
        this->recomputeTransform();
    }

    void Camera::setPosition(const glm::vec2 &position)
    {
        this->position = position;
        this->recomputeTransform();
    }

    void Camera::recomputeTransform()
    {
        float scale = 1.0;
        this->transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));

        // translation - Inverting coorinates because translation matrix has to 'move' the objects into the different direction
        // translation is clipped to integers
        glm::vec3 translation(
            floor(-(this->position.x - this->viewport.width / 2) * scale) / scale,
            floor(-(this->position.y - this->viewport.height / 2) * scale) / scale,
            0.0
        );
        this->transform = glm::translate(this->transform, translation);
    }
} // namespace engine