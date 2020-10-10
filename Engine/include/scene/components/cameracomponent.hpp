#pragma once

#include <glm/glm.hpp>

#include "core/viewport.hpp"

namespace engine
{
    struct CameraComponent
    {
        glm::mat4 transform;
        const Viewport *viewport;

        CameraComponent();
        CameraComponent(const CameraComponent &) = default;
        CameraComponent(const glm::mat4 &transform, const Viewport *viewport)
            : transform(transform), viewport(viewport) {}

        operator glm::mat4 &() { return this->transform; }
        operator const glm::mat4 &() const { return this->transform; }
    };
} // namespace engine