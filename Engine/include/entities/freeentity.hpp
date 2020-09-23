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

        virtual void update(const double deltaTime) override;
        virtual void moveLeft() override;
        virtual void moveRight() override;
        virtual void moveUp() override;
        virtual void moveDown() override;
    };
} // namespace engine

#endif