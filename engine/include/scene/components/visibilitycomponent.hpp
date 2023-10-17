#pragma once

namespace engine
{
    struct VisibilityComponent
    {
    private:
        bool visible = true;

    public:
        VisibilityComponent() = default;
        VisibilityComponent(const VisibilityComponent &visibilityComponent) = default;
    };
} // namespace engine