#pragma once

namespace tilegame::components
{
    struct Movement
    {
        enum MovementDirection : char
        {
            None = 0,
            Left = 1 << 1,
            Right = 1 << 2,
            Up = 1 << 3,
            Down = 1 << 4
        };

        MovementDirection direction;
        float speed;
    };

    inline Movement::MovementDirection operator|(Movement::MovementDirection a, Movement::MovementDirection b)
    {
        return static_cast<Movement::MovementDirection>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline void operator|=(Movement::MovementDirection &a, Movement::MovementDirection b)
    {
        a = a | b;
    }

    inline Movement::MovementDirection operator&(Movement::MovementDirection a, Movement::MovementDirection b)
    {
        return static_cast<Movement::MovementDirection>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline void operator&=(Movement::MovementDirection &a, Movement::MovementDirection b)
    {
        a = a & b;
    }
} // namespace tilegame
