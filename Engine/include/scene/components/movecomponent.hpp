#pragma once

#include <glm/glm.hpp>

namespace engine
{
    struct MoveComponent
    {
        enum class MoveDirection
        {
            Left = 1,
            Right = 2,
            Up = 4,
            Down = 8
        };

        friend MoveDirection operator|(const MoveDirection &a, const MoveDirection &b)
        {
            return static_cast<MoveDirection>(static_cast<int>(a) | static_cast<int>(b));
        }

        friend MoveDirection operator&(const MoveDirection &a, const MoveDirection &b)
        {
            return static_cast<MoveDirection>(static_cast<int>(a) & static_cast<int>(b));
        }

        friend MoveDirection &operator|=(MoveDirection &a, const MoveDirection &b)
        {
            a = a | b;
            return a;
        }

        MoveDirection direction;
        double speed;

        MoveComponent() = default;
        MoveComponent(const MoveComponent &) = default;
        MoveComponent(const MoveDirection &direction, const double speed)
            : direction(direction), speed(speed) {}
    };
} // namespace engine