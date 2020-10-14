#include "scene/components/cameracomponent.hpp"

namespace engine
{
    CameraComponent::CameraComponent() : CameraComponent(glm::mat4(1.0), nullptr)
    {
    }
} // namespace engine