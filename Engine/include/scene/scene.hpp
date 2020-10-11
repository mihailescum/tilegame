#pragma once

#include "entt.hpp"

#include "core/game.hpp"

namespace engine
{
    class Entity;
    class Scene {
        protected:
            entt::registry registry;
            Game &game;

        public:
            Scene(Game &game);
            Scene(const Scene &scene) = delete;

            const Entity createEntity();
            entt::registry &getRegistry();

            template<class Component>
            Component &getComponent(entt::entity entity) {
                return this->registry.get<Component>(entity);
            }

            virtual void initialize() {};
            virtual void loadContent() {};
            virtual void unloadContent() {};
            virtual void processInput() {};
            virtual void update(const double deltaTime) {};
            virtual void draw() {};
    };
}