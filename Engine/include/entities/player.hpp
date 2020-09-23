#ifndef __ENGINE_PLAYER_H__
#define __ENGINE_PLAYER_H__

#include <memory>
#include "glm/glm.hpp"

#include "core/camera.hpp"
#include "entities/entity.hpp"

namespace engine
{
    class Player
    {
    private:
        std::unique_ptr<Camera> camera;
        Entity *entity;

    public:
        Player(Entity *entity, const Viewport& viewport);

        void update(const double deltaTime);
        void moveLeft();
        void moveRight();
        void moveUp();
        void moveDown();

        const Camera& getCamera() const;
    };
} // namespace engine

#endif