#pragma once

#include "entt.hpp"
#include "game.hpp"

namespace engine
{
    class Entity;
    class Scene
    {
    protected:
        Game &game;

    public:
        Scene(Game &game);
        Scene(const Scene &scene) = delete;

        // entt::registry &Scene::getRegistry() { return this->registry; }

        const Game &getGame() const;

        virtual void initialize(){};
        virtual void loadContent(){};
        virtual void unloadContent(){};
        virtual void processInput(){};
        virtual void update(const double deltaTime){};
        virtual void draw(){};
    };
}