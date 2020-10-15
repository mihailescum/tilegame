#pragma once

namespace engine
{
    struct VisiblityComponent
    {
    private:
        bool visible = true;

    public:
        VisiblityComponent() = default;
        VisiblityComponent(const VisiblityComponent &visiblityComponent) = default;
    };
} // namespace engine