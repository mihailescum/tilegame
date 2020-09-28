#pragma once

#include "entt.hpp"

namespace engine
{
    class Entity;
    class Scene {
        friend class Entity;
        
        private:
            entt::registry registry;
        public:
            Scene();
            const Entity createEntity();
    };
}