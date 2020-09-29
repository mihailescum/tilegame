#pragma once

#include "entt.hpp"

namespace engine
{
    class Entity;
    class Scene {
        private:
            entt::registry registry;
        public:
            Scene();
            const Entity createEntity();
            entt::registry &getRegistry();

            template<class Component>
            Component &getComponent(entt::entity entity) {
                return this->registry.get<Component>(entity);
            }
    };
}