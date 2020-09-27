#pragma once

#include "glm/glm.hpp"

namespace engine
{
    class Entity
    {
    protected:
        glm::vec2 position;

    public:
        Entity();

        virtual void update(const double deltaTime) {}
        virtual void moveLeft() {}
        virtual void moveRight() {}
        virtual void moveUp() {}
        virtual void moveDown() {}

        const glm::vec2 &getPosition() const;

        enum class MovingDirection
        {
            Left = 1,
            Right = 2,
            Up = 4,
            Down = 8
        };

        friend MovingDirection operator|(const MovingDirection &a, const MovingDirection &b) {
            return static_cast<MovingDirection>(static_cast<int>(a) | static_cast<int>(b));
        }

        friend MovingDirection operator&(const MovingDirection &a, const MovingDirection &b) {
            return static_cast<MovingDirection>(static_cast<int>(a) & static_cast<int>(b));
        }

        friend MovingDirection &operator|=(MovingDirection &a, const MovingDirection &b) {
            a = a | b;
            return a;
        }
    };
} // namespace engine
