#pragma once

#include "system.hpp"

namespace tilegame::systems
{
    class ParticleSystem : public System
    {
    public:
        ParticleSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
