#pragma once

#include <vector>

#include "system.hpp"

namespace tilegame::systems
{
    class Timer : public System
    {
    private:
        std::vector<entt::entity> _entities_to_clear;

    public:
        Timer(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame