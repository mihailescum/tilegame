#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class Camera : public System
    {
    public:
        Camera(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame