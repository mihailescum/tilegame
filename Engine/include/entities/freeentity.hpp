#ifndef __ENGINE_FREEENTITY_H__
#define __ENGINE_FREEENTITY_H__

#include "entities/entity.hpp"

namespace engine
{
    class FreeEntity : public Entity
    {
    private:
        double speed = 32.0;
        MovingDirection movingDirection;

    public:
        FreeEntity();

        virtual void update(const double deltaTime);
        virtual void moveLeft();
        virtual void moveRight();
        virtual void moveUp();
        virtual void moveDown();
    };
} // namespace engine

#endif