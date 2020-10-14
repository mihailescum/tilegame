#pragma once

namespace engine
{
    struct RenderComponent
    {
        RenderComponent() = default;
        RenderComponent(const RenderComponent &RenderComponent) = default;
        RenderComponent(const int width, const int height, const double z) : width(width), height(height), z(z) {}

        int width;
        int height;
        double z;
    };
} // namespace engine