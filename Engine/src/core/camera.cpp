#include "core/camera.hpp"

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
        // translation - Inverting coorinates because translation matrix has to 'move' the objects into the different direction
        // translation is clipped to integers
        this->transform[3][0] = -(int)(this->position.x - this->viewport.width / 2);
        this->transform[3][1] = -(int)(this->position.y - this->viewport.height / 2);
    }
} // namespace engine