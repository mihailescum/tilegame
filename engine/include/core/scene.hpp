#pragma once

#include "entt.hpp"
#include "game.hpp"

namespace engine
{
    class Entity;
    class Scene
    {
    protected:
        Game &_game;

    public:
        Scene(Game &game);
        Scene(const Scene &scene) = delete;

        const Game &get_game() const;

        virtual void initialize(){};
        virtual void load_content(){};
        virtual void unload_content(){};
        virtual void update(const double deltaTime){};
        virtual void draw(){};
    };
}