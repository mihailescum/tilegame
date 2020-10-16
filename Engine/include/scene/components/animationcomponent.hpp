#pragma once

namespace engine
{
    struct AnimationComponent
    {
    public:
        AnimationComponent() = default;
        AnimationComponent(const AnimationComponent &component) = default;
        AnimationComponent(const double tickDuration) : tickDuration(tickDuration), timePassed(0.0) , doAnimation(false) {}

        double tickDuration;
        double timePassed;
        bool doAnimation;
    };
} // namespace engine